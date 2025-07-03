#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/MaterialImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialAsset, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialAsset::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialAsset::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialAsset::_defaultInstanceParams, "_defaultInstanceParams")->AddTrait<ReflectionTraitHide>();
	}

	DESCRIBE_REFLECTED_CLASS(MaterialImporterSettings, reflectionDescriptor)
	{

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
	bool MaterialImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		char* dataBuffer = DefaultAllocator::GetInstance().Allocate<char>(dataSize + 1);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != dataSize)
		{
			OUTPUT_ERROR("MaterialImporter : Can't read Shader data");
			return false;
		}
		dataBuffer[dataSize] = '\0';

		std::string shaderTypeParams;
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

		std::filesystem::path slangVertexOutput = FileSystem::GetTemporaryPath() / "HodShaderVertexImporter.tmp";
		std::string stageVertexParameter = "-entry VertexMain -stage vertex";
#if defined(PLATFORM_WINDOWS)
		bool slangResult = Process::Create("Tools/slangc.exe", std::format("{} -target spirv {} -o {}", data._path.string(), stageVertexParameter, slangVertexOutput.string()), false);
#elif defined(PLATFORM_LINUX)
		bool slangResult = Process::Create("Tools/slangc", std::format("{} -target spv {} -o {}", data._path.string(), stageVertexParameter, slangVertexOutput.string()), false);
#else
		bool slangResult = Process::Create("Tools/slangc", std::format("{} -target metallib {} -o {}", data._path.string(), stageVertexParameter, slangVertexOutput.string()), false);
#endif
		if (slangResult == false)
		{
			// todo message
			return false;
		}

		std::filesystem::path slangFragmentOutput = FileSystem::GetTemporaryPath() / "HodShaderFragmentImporter.tmp";
		std::string stageFragmentParameter = "-entry FragmentMain -stage fragment";
#if defined(PLATFORM_WINDOWS)
		slangResult = Process::Create("Tools/slangc.exe", std::format("{} -target spirv {} -o {}", data._path.string(), stageFragmentParameter, slangFragmentOutput.string()), false);
#elif defined(PLATFORM_LINUX)
		slangResult = Process::Create("Tools/slangc", std::format("{} -target spv {} -o {}", data._path.string(), stageFragmentParameter, slangFragmentOutput.string()), false);
#else
		slangResult = Process::Create("Tools/slangc", std::format("{} -target metallib {} -o {}", data._path.string(), stageFragmentParameter, slangFragmentOutput.string()), false);
#endif
		if (slangResult == false)
		{
			// todo message
			return false;
		}

		FileSystem::Handle vertexSlangOutputHandle = FileSystem::GetInstance()->Open(slangVertexOutput);
		uint32_t vertexDataSize = FileSystem::GetInstance()->GetSize(vertexSlangOutputHandle);
		char* vertexDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(vertexDataSize + 1);
		if (FileSystem::GetInstance()->Read(vertexSlangOutputHandle, reinterpret_cast<char*>(vertexDataBuffer), vertexDataSize) != vertexDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			FileSystem::GetInstance()->Close(vertexSlangOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Vertex output");
			return false;
		}
		FileSystem::GetInstance()->Close(vertexSlangOutputHandle);
		vertexDataBuffer[vertexDataSize] = '\0';

		FileSystem::Handle fragmentSlangOutputHandle = FileSystem::GetInstance()->Open(slangFragmentOutput);
		uint32_t fragmentDataSize = FileSystem::GetInstance()->GetSize(fragmentSlangOutputHandle);
		char* fragmentDataBuffer = DefaultAllocator::GetInstance().Allocate<char>(fragmentDataSize + 1);
		if (FileSystem::GetInstance()->Read(fragmentSlangOutputHandle, reinterpret_cast<char*>(fragmentDataBuffer), fragmentDataSize) != fragmentDataSize)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			FileSystem::GetInstance()->Close(fragmentSlangOutputHandle);
			OUTPUT_ERROR("MaterialImporter : Can't read Fragment output");
			return false;
		}
		FileSystem::GetInstance()->Close(fragmentSlangOutputHandle);
		fragmentDataBuffer[fragmentDataSize] = '\0';

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, meta) == false)
		{
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			return false;
		}
		document.GetRootNode().AddChild("VertexDataOffset").SetUInt32(0);
		document.GetRootNode().AddChild("VertexDataSize").SetUInt32(vertexDataSize);
		document.GetRootNode().AddChild("FragmentDataOffset").SetUInt32(vertexDataSize);
		document.GetRootNode().AddChild("FragmentDataSize").SetUInt32(fragmentDataSize);
		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			return false;
		}

		uint32_t documentLen = (uint32_t)documentStringStream.str().size();
		resource.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		// todo use documentStringStream ?
		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			DefaultAllocator::GetInstance().Free(vertexDataBuffer);
			DefaultAllocator::GetInstance().Free(fragmentDataBuffer);
			return false;
		}
		resource.write(reinterpret_cast<char*>(vertexDataBuffer), vertexDataSize);
		DefaultAllocator::GetInstance().Free(vertexDataBuffer);
		resource.write(reinterpret_cast<char*>(fragmentDataBuffer), fragmentDataSize);
		DefaultAllocator::GetInstance().Free(fragmentDataBuffer);

		if (resource.fail())
		{
			// TODO message
			return false;
		}

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
		return &renderer::MaterialResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> MaterialImporter::AllocateSettings() const
	{
		return std::make_shared<MaterialImporterSettings>();
	}
}
