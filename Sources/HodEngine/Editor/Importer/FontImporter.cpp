#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/FontImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(FontImporterSettings, reflectionDescriptor)
	{
	}

	/// @brief 
	FontImporter::FontImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("ttf");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool FontImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		uint8_t* dataBuffer = new uint8_t[dataSize];
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != dataSize)
		{
			OUTPUT_ERROR("FontImporter : Can't read Font data");
			return false;
		}

		//FontImporterSettings& fontSettings = (FontImporterSettings&)settings;

		Document document;
		document.GetRootNode().AddChild("DataOffset").SetUInt32(0);
		document.GetRootNode().AddChild("DataSize").SetUInt32((uint32_t)dataSize);

		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			return false;
		}

		uint32_t documentLen = (uint32_t)documentStringStream.str().size();
		resource.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		// todo use documentStringStream ?
		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			return false;
		}
		resource.write(reinterpret_cast<char*>(dataBuffer), dataSize);

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
	const char* FontImporter::GetTypeName() const
	{
		return "FontImporter";
	}

	ReflectionDescriptor* FontImporter::GetResourceDescriptor() const
	{
		return renderer::FontResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> FontImporter::AllocateSettings() const
	{
		return std::make_shared<FontImporterSettings>();
	}
}
