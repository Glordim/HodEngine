#include "HodEngine/Editor/Importer/PrefabImporter.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(PrefabImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool PrefabImporter::WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
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

	/// @brief 
	/// @return 
	const char* PrefabImporter::GetTypeName() const
	{
		return "PrefabImporter";
	}

	ImporterSettings* PrefabImporter::AllocateSettings() const
	{
		return new PrefabImporterSettings();
	}

	ReflectionDescriptor* PrefabImporter::GetResourceDescriptor() const
	{
		return game::PrefabResource::GetReflectionDescriptor();
	}
}
