#include "HodEngine/Editor/Importer/FontImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(FontImporterSettings, ImporterSettings)
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
	bool FontImporter::WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		data.seekg(0, std::ios::end);
		std::streampos dataSize = data.tellg();
		data.seekg(0, std::ios::beg);
		uint8_t* dataBuffer = new uint8_t[dataSize];
		data.read(reinterpret_cast<char*>(dataBuffer), dataSize); // try ?
		if (data.fail())
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

	ImporterSettings* FontImporter::AllocateSettings() const
	{
		return new FontImporterSettings();
	}
}
