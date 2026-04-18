#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/Importer/MaterialImporter.hpp"

#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>

#include <sstream>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialImporterSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_defaultInstanceParams, "_defaultInstanceParams")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief
	MaterialImporter::MaterialImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("slang");
	}

	/// @brief
	/// @param path
	/// @return
	bool MaterialImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail,
	                                     ImporterSettings& settings)
	{
		(void)thumbnail; // TODO
		(void)settings;

		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		char*    dataBuffer = DefaultAllocator::GetInstance().Allocate<char>(dataSize + 1);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != (int32_t)dataSize)
		{
			OUTPUT_ERROR("MaterialImporter : Can't read Shader data");
			return false;
		}
		dataBuffer[dataSize] = '\0';

		String shaderTypeParams;
		if (std::strstr(dataBuffer, "VertexMain") == nullptr)
		{
			DefaultAllocator::GetInstance().Free(dataBuffer);
			OUTPUT_ERROR("MaterialImporter : Can't find VertexMain entry point");
			return false;
		}
		if (std::strstr(dataBuffer, "FragmentMain") == nullptr)
		{
			DefaultAllocator::GetInstance().Free(dataBuffer);
			OUTPUT_ERROR("MaterialImporter : Can't find FragmentMain entry point");
			return false;
		}
		DefaultAllocator::GetInstance().Free(dataBuffer); // todo remove (link with slang and don't use filesystem)

		Path   slangVertexOutput = FileSystem::GetTemporaryPath() / "HodShaderVertexImporter.tmp";
		Path   slangVertexReflectionOutput = FileSystem::GetTemporaryPath() / "HodShaderVertexReflectionImporter.tmp";
		String stageVertexParameter = "-entry VertexMain -stage vertex";
#if defined(PLATFORM_WINDOWS)
		bool slangResult = Process::Create("Tools/slangc.exe", fmt::format("{} -target spirv {} -o {} -reflection-json {}", data._path, stageVertexParameter, slangVertexOutput, slangVertexReflectionOutput), false);
#elif defined(PLATFORM_LINUX)
		bool slangResult = Process::Create("Tools/slangc", fmt::format("{} -target spv {} -o {} -reflection-json {}", data._path, stageVertexParameter, slangVertexOutput, slangVertexReflectionOutput), false);
#else
		bool slangResult = Process::Create("Tools/slangc", fmt::format("{} -target metallib {} -o {} -reflection-json {}", data._path, stageVertexParameter, slangVertexOutput, slangVertexReflectionOutput), false);
#endif
		if (slangResult == false)
		{
			// todo message
			return false;
		}

		Path   slangFragmentOutput = FileSystem::GetTemporaryPath() / "HodShaderFragmentImporter.tmp";
		Path   slangFragmentReflectionOutput = FileSystem::GetTemporaryPath() / "HodShaderFragmentReflectionImporter.tmp";
		String stageFragmentParameter = "-entry FragmentMain -stage fragment";
#if defined(PLATFORM_WINDOWS)
		slangResult = Process::Create("Tools/slangc.exe", fmt::format("{} -target spirv {} -o {} -reflection-json {}", data._path, stageFragmentParameter, slangFragmentOutput, slangFragmentReflectionOutput), false);
#elif defined(PLATFORM_LINUX)
		slangResult = Process::Create("Tools/slangc", fmt::format("{} -target spv {} -o {} -reflection-json {}", data._path, stageFragmentParameter, slangFragmentOutput, slangFragmentReflectionOutput), false);
#else
		slangResult = Process::Create("Tools/slangc", fmt::format("{} -target metallib {} -o {} -reflection-json {}", data._path, stageFragmentParameter, slangFragmentOutput, slangFragmentReflectionOutput), false);
#endif
		if (slangResult == false)
		{
			// todo message
			return false;
		}

		FileSystem::Handle vertexSlangOutputHandle = FileSystem::GetInstance()->Open(slangVertexOutput);
		uint32_t           vertexDataSize = FileSystem::GetInstance()->GetSize(vertexSlangOutputHandle);
		char*              vertexDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(vertexDataSize + 1);
		if (FileSystem::GetInstance()->Read(vertexSlangOutputHandle, reinterpret_cast<char*>(vertexDataBuffer), vertexDataSize) != (int32_t)vertexDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			FileSystem::GetInstance()->Close(vertexSlangOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Vertex output");
			return false;
		}
		FileSystem::GetInstance()->Close(vertexSlangOutputHandle);
		vertexDataBuffer[vertexDataSize] = '\0';

		FileSystem::Handle vertexSlangReflectionOutputHandle = FileSystem::GetInstance()->Open(slangVertexReflectionOutput);
		uint32_t           vertexReflectionDataSize = FileSystem::GetInstance()->GetSize(vertexSlangReflectionOutputHandle);
		char*              vertexReflectionDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(vertexReflectionDataSize + 1);
		if (FileSystem::GetInstance()->Read(vertexSlangReflectionOutputHandle, reinterpret_cast<char*>(vertexReflectionDataBuffer), vertexReflectionDataSize) != (int32_t)vertexReflectionDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(vertexReflectionDataBuffer);
			FileSystem::GetInstance()->Close(vertexSlangReflectionOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Vertex Reflection output");
			return false;
		}
		FileSystem::GetInstance()->Close(vertexSlangReflectionOutputHandle);
		vertexReflectionDataBuffer[vertexReflectionDataSize] = '\0';

		FileSystem::Handle fragmentSlangOutputHandle = FileSystem::GetInstance()->Open(slangFragmentOutput);
		uint32_t           fragmentDataSize = FileSystem::GetInstance()->GetSize(fragmentSlangOutputHandle);
		char*              fragmentDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(fragmentDataSize + 1);
		if (FileSystem::GetInstance()->Read(fragmentSlangOutputHandle, reinterpret_cast<char*>(fragmentDataBuffer), fragmentDataSize) != (int32_t)fragmentDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(vertexReflectionDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			FileSystem::GetInstance()->Close(fragmentSlangOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Fragment output");
			return false;
		}
		FileSystem::GetInstance()->Close(fragmentSlangOutputHandle);
		fragmentDataBuffer[fragmentDataSize] = '\0';

		FileSystem::Handle fragmentSlangReflectionOutputHandle = FileSystem::GetInstance()->Open(slangFragmentReflectionOutput);
		uint32_t           fragmentReflectionDataSize = FileSystem::GetInstance()->GetSize(fragmentSlangReflectionOutputHandle);
		char*              fragmentReflectionDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(fragmentReflectionDataSize + 1);
		if (FileSystem::GetInstance()->Read(fragmentSlangReflectionOutputHandle, reinterpret_cast<char*>(fragmentReflectionDataBuffer), fragmentReflectionDataSize) != (int32_t)fragmentReflectionDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(vertexReflectionDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentReflectionDataBuffer);
			FileSystem::GetInstance()->Close(fragmentSlangReflectionOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Fragment Reflection output");
			return false;
		}
		FileSystem::GetInstance()->Close(fragmentSlangReflectionOutputHandle);
		fragmentReflectionDataBuffer[fragmentReflectionDataSize] = '\0';

		Document           metaDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(metaDocument, meta) == false) // todo dont copy all meta
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(vertexReflectionDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentReflectionDataBuffer);
			return false;
		}
		const DocumentNode* importerSettings = metaDocument.GetRootNode().GetChild("importerSettings");
		if (importerSettings == nullptr)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(vertexReflectionDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentReflectionDataBuffer);
			return false;
		}
		document.GetRootNode().Copy(*importerSettings);
		document.GetRootNode().SetName("");

		Resource::Data vertexData;
		vertexData._buffer = vertexDataBuffer;
		vertexData._size = vertexDataSize;
		datas.push_back(vertexData);

		Resource::Data vertexReflectionData;
		vertexReflectionData._buffer = vertexReflectionDataBuffer;
		vertexReflectionData._size = vertexReflectionDataSize;
		datas.push_back(vertexReflectionData);

		Resource::Data fragmentData;
		fragmentData._buffer = fragmentDataBuffer;
		fragmentData._size = fragmentDataSize;
		datas.push_back(fragmentData);

		Resource::Data fragmentReflectionData;
		fragmentReflectionData._buffer = fragmentReflectionDataBuffer;
		fragmentReflectionData._size = fragmentReflectionDataSize;
		datas.push_back(fragmentReflectionData);

		return true;
	}

	// TODO Move all virtual in Ctor const init ?

	/// @brief
	/// @return
	const char* MaterialImporter::GetTypeName() const
	{
		return "MaterialImporter";
	}

	ReflectionDescriptor* MaterialImporter::GetResourceDescriptor() const
	{
		return &MaterialResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> MaterialImporter::AllocateSettings() const
	{
		return std::make_shared<MaterialImporterSettings>();
	}
}
