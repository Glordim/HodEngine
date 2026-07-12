#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialCooker.hpp"

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/ScopedGuard.hpp>
#include <HodEngine/Core/UID.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <utility>

namespace hod::inline editor
{
	namespace
	{
		/// @brief Rewrite, under outputDir, every slang source registered on the asset (root file first, then its
		/// transitive includes), mirroring their original relative layout so slangc can resolve #include directives
		bool ReconstituteSlangSources(const AssetContainer& assetContainer, const Path& outputDir, Path& outRootPath)
		{
			const Vector<AssetContainer::SourceInfo>& sources = assetContainer.GetSources();
			if (sources.Empty())
			{
				OUTPUT_ERROR("MaterialCooker: asset has no registered source");
				return false;
			}

			Path rootDir = sources[0]._path.ParentPath();

			for (uint32_t i = 0; i < sources.Size(); ++i)
			{
				Path relativePath = sources[i]._path.LexicallyRelative(rootDir);
				relativePath.PortableSeparator();

				const AssetContainer::DataBlockInfo* dataBlock = assetContainer.FindDataBlock(relativePath.GetString());
				if (dataBlock == nullptr)
				{
					OUTPUT_ERROR("MaterialCooker: missing data block for '{}'", relativePath);
					return false;
				}

				String content;
				content.Resize(dataBlock->_uncompressedSize);
				if (dataBlock->_stream->Read(content.Data(), content.Size()) != content.Size())
				{
					OUTPUT_ERROR("MaterialCooker: can't read data block for '{}'", relativePath);
					return false;
				}

				Path filePath = outputDir / relativePath;
				if (FileSystem::GetInstance()->CreateDirectories(filePath.ParentPath()) == false
					|| FileSystem::GetInstance()->WriteAllText(filePath, content) == false)
				{
					OUTPUT_ERROR("MaterialCooker: can't write reconstituted source '{}'", filePath);
					return false;
				}

				if (i == 0)
				{
					outRootPath = filePath;
				}
			}

			return true;
		}

		/// @brief Read a whole file into memory, for picking up slangc's output/reflection files
		bool ReadBinaryFile(const Path& path, Vector<uint8_t>& outData)
		{
			FileSystem::Handle handle = FileSystem::GetInstance()->Open(path);
			if (handle.IsOpen() == false)
			{
				return false;
			}

			uint32_t size = FileSystem::GetInstance()->GetSize(handle);
			outData.Resize(size);
			bool result = FileSystem::GetInstance()->Read(handle, outData.Data(), size) == static_cast<int32_t>(size);
			FileSystem::GetInstance()->Close(handle);
			return result;
		}
	}

	bool MaterialCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)platforms;
		(void)configs;
		(void)languages;

		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		Path slangDir = FileSystem::GetTemporaryPath() / "HodEngine" / Project::GetInstance()->GetName() / "Cook" / asset.GetUid().ToString() / "Slang";
		ScopedGuard cleanupSlangDir = [&]() { FileSystem::GetInstance()->RemoveAll(slangDir); };
		if (FileSystem::GetInstance()->CreateDirectories(slangDir) == false)
		{
			OUTPUT_ERROR("MaterialCooker: unable to create tmp dir '{}'", slangDir);
			return false;
		}

		Path slangRootPath;
		if (ReconstituteSlangSources(assetContainer, slangDir, slangRootPath) == false)
		{
			return false;
		}

		String rootContent;
		if (FileSystem::GetInstance()->ReadAllText(slangRootPath, rootContent) == false)
		{
			OUTPUT_ERROR("MaterialCooker: can't read '{}'", slangRootPath);
			return false;
		}
		if (rootContent.Find("VertexMain") == String::Npos)
		{
			OUTPUT_ERROR("MaterialCooker: can't find VertexMain entry point");
			return false;
		}
		if (rootContent.Find("FragmentMain") == String::Npos)
		{
			OUTPUT_ERROR("MaterialCooker: can't find FragmentMain entry point");
			return false;
		}

		if (CompileSlangStage(slangRootPath, "VertexMain", "vertex", "Vertex") == false)
		{
			return false;
		}
		if (CompileSlangStage(slangRootPath, "FragmentMain", "fragment", "Fragment") == false)
		{
			return false;
		}

		return true;
	}

	bool MaterialCooker::CompileSlangStage(const Path& slangRootPath, std::string_view entryPoint, std::string_view stage, std::string_view dataBlockName)
	{
		Path   stageOutput = slangRootPath.ParentPath() / (String(dataBlockName) + ".bin");
		Path   stageReflectionOutput = slangRootPath.ParentPath() / (String(dataBlockName) + ".reflection.json");
		String stageParameter(fmt::format("-entry {} -stage {}", entryPoint, stage));
#if defined(PLATFORM_WINDOWS)
		bool slangResult = Process::Create("Tools/slangc.exe", fmt::format("{} -target spirv -profile glsl_450+spirv_1_3 {} -o {} -reflection-json {}", slangRootPath, stageParameter, stageOutput, stageReflectionOutput), false);
#elif defined(PLATFORM_LINUX)
		bool slangResult = Process::Create("Tools/slangc", fmt::format("{} -target spirv -profile glsl_450+spirv_1_3 {} -o {} -reflection-json {}", slangRootPath, stageParameter, stageOutput, stageReflectionOutput), false);
#else
		bool slangResult = Process::Create("Tools/slangc", fmt::format("{} -target metallib {} -o {} -reflection-json {}", slangRootPath, stageParameter, stageOutput, stageReflectionOutput), false);
#endif
		if (slangResult == false)
		{
			OUTPUT_ERROR("MaterialCooker: slangc failed for '{}' stage", stage);
			return false;
		}

		Vector<uint8_t> stageData;
		if (ReadBinaryFile(stageOutput, stageData) == false)
		{
			OUTPUT_ERROR("MaterialCooker: can't read slangc output '{}'", stageOutput);
			return false;
		}

		Vector<uint8_t> stageReflectionData;
		if (ReadBinaryFile(stageReflectionOutput, stageReflectionData) == false)
		{
			OUTPUT_ERROR("MaterialCooker: can't read slangc reflection '{}'", stageReflectionOutput);
			return false;
		}

		// Compiled against the host toolchain's target (spirv/metallib), not per requested platform yet
		Stream& stageStream = AddDataBlockStream(dataBlockName, true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		stageStream.Write(stageData.Data(), stageData.Size());

		Stream& stageReflectionStream = AddDataBlockStream(String(dataBlockName) + "Reflection", true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		stageReflectionStream.Write(stageReflectionData.Data(), stageReflectionData.Size());

		return true;
	}
}
