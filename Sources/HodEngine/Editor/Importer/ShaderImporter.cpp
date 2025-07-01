#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/ShaderImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/ShaderResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>

#include <sstream>
#include <cstring>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(ShaderImporterSettings, reflectionDescriptor)
	{
	}

	/// @brief 
	ShaderImporter::ShaderImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("vert", "frag", "slang");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool ShaderImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		char* dataBuffer = DefaultAllocator::GetInstance().Allocate<char>(dataSize + 1);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != dataSize)
		{
			OUTPUT_ERROR("ShaderImporter : Can't read Shader data");
			return false;
		}
		dataBuffer[dataSize] = '\0';

		//ShaderImporterSettings& fontSettings = (ShaderImporterSettings&)settings;

		std::string shaderTypeParams;
		renderer::Shader::ShaderType shaderType;
		if (std::strstr(dataBuffer, "VertexMain") != NULL)
		{
			shaderType = renderer::Shader::ShaderType::Vertex;
			shaderTypeParams = "-entry VertexMain -stage vertex";
		}
		else if (std::strstr(dataBuffer, "FragmentMain") != NULL)
		{
			shaderType = renderer::Shader::ShaderType::Fragment;
			shaderTypeParams = "-entry FragmentMain -stage fragment";
		}
		else
		{
			// todo message
			return false;
		}

		DefaultAllocator::GetInstance().Free(dataBuffer);

		std::filesystem::path slangOutput = FileSystem::GetTemporaryPath() / "ShaderImporter.tmp";

#if defined(PLATFORM_WINDOWS)
		bool slangResult = Process::Create("Tools/slangc.exe", std::format("{} -target spirv {} -o {}", data._path.string(), shaderTypeParams, slangOutput.string()), false);
#elif defined(PLATFORM_LINUX)
		bool slangResult = Process::Create("Tools/slangc", std::format("{} -target spv {} -o {}", data._path.string(), shaderTypeParams, slangOutput.string()), false);
#else
		bool slangResult = Process::Create("Tools/slangc", std::format("{} -target metallib {} -o {}", data._path.string(), shaderTypeParams, slangOutput.string()), false);
#endif

		if (slangResult == false)
		{
			// todo message
			return false;
		}

		FileSystem::Handle slangOutputHandle = FileSystem::GetInstance()->Open(slangOutput);
		dataSize = FileSystem::GetInstance()->GetSize(slangOutputHandle);
		dataBuffer = DefaultAllocator::GetInstance().Allocate<char>(dataSize + 1);
		if (FileSystem::GetInstance()->Read(slangOutputHandle, reinterpret_cast<char*>(dataBuffer), dataSize) != dataSize)
		{
			DefaultAllocator::GetInstance().Free(dataBuffer);
			FileSystem::GetInstance()->Close(slangOutputHandle);
			OUTPUT_ERROR("ShaderImporter : Can't read Shader data");
			return false;
		}
		FileSystem::GetInstance()->Close(slangOutputHandle);
		dataBuffer[dataSize] = '\0';

		Document document;
		document.GetRootNode().AddChild("_type").SetUInt8(shaderType);
		document.GetRootNode().AddChild("DataOffset").SetUInt32(0);
		document.GetRootNode().AddChild("DataSize").SetUInt32(dataSize);
		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			DefaultAllocator::GetInstance().Free(dataBuffer);
			return false;
		}

		uint32_t documentLen = (uint32_t)documentStringStream.str().size();
		resource.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		// todo use documentStringStream ?
		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			DefaultAllocator::GetInstance().Free(dataBuffer);
			return false;
		}
		resource.write(reinterpret_cast<char*>(dataBuffer), dataSize);
		DefaultAllocator::GetInstance().Free(dataBuffer);

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
	const char* ShaderImporter::GetTypeName() const
	{
		return "ShaderImporter";
	}

	ReflectionDescriptor* ShaderImporter::GetResourceDescriptor() const
	{
		return &renderer::ShaderResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> ShaderImporter::AllocateSettings() const
	{
		return std::make_shared<ShaderImporterSettings>();
	}
}
