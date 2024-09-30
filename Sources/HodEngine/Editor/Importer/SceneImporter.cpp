#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(SceneImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool SceneImporter::WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
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
	const char* SceneImporter::GetTypeName() const
	{
		return "SceneImporter";
	}

	std::shared_ptr<ImporterSettings> SceneImporter::AllocateSettings() const
	{
		return std::make_shared<SceneImporterSettings>();
	}

	ReflectionDescriptor* SceneImporter::GetResourceDescriptor() const
	{
		return nullptr;
	}
}
