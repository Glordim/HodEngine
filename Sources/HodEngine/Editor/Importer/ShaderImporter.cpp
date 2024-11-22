#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/ShaderImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/ShaderResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(ShaderImporterSettings, reflectionDescriptor)
	{
	}

	/// @brief 
	ShaderImporter::ShaderImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("vert", "frag");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool ShaderImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		char* dataBuffer = new char[dataSize + 1];
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != dataSize)
		{
			OUTPUT_ERROR("ShaderImporter : Can't read Shader data");
			return false;
		}
		dataBuffer[dataSize] = '\0';

		//ShaderImporterSettings& fontSettings = (ShaderImporterSettings&)settings;

		renderer::Shader::ShaderType shaderType;
		if (strstr(dataBuffer, "VertexMain") != NULL)
		{
			shaderType = renderer::Shader::ShaderType::Vertex;
		}
		else if (strstr(dataBuffer, "FragMain") != NULL)
		{
			shaderType = renderer::Shader::ShaderType::Fragment;
		}
		else
		{
			// todo message
			return false;
		}

		Document document;
		document.GetRootNode().AddChild("_type").SetUInt8(shaderType);
		document.GetRootNode().AddChild("_source").SetString(dataBuffer);

		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			return false;
		}
		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			return false;
		}

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
		return renderer::ShaderResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> ShaderImporter::AllocateSettings() const
	{
		return std::make_shared<ShaderImporterSettings>();
	}
}
