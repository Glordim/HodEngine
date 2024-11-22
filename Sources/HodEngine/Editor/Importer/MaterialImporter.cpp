#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/MaterialImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialAsset, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &MaterialAsset::_vertexShader, "_vertexShader");
		AddPropertyT(&reflectionDescriptor, &MaterialAsset::_fragmentShader, "_fragmentShader");

		AddPropertyT(&reflectionDescriptor, &MaterialAsset::_polygonMode, "_polygonMode");
		AddPropertyT(&reflectionDescriptor, &MaterialAsset::_topololy, "_topololy");
	}

	DESCRIBE_REFLECTED_CLASS(MaterialImporterSettings, reflectionDescriptor)
	{

	}

	/// @brief 
	MaterialImporter::MaterialImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("mat");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool MaterialImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
		{
			return false;
		}

		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			return false;
		}

		uint32_t documentLen = (uint32_t)documentStringStream.str().size();
		resource.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		// todo use documentStringStream ?
		if (documentWriter.Write(document, resource) == false)
		{
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
		return renderer::MaterialResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> MaterialImporter::AllocateSettings() const
	{
		return std::make_shared<MaterialImporterSettings>();
	}
}
