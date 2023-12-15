#include "HodEngine/Editor/Importer/DefaultImporter.hpp"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(DefaultImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(FileStream& data, FileStream& meta, FileStream& resource, FileStream& thumbnail, ImporterSettings& settings)
	{
		return false;
	}

	/// @brief 
	/// @return 
	const char* DefaultImporter::GetTypeName() const
	{
		return "DefaultImporter";
	}

	ImporterSettings* DefaultImporter::AllocateSettings() const
	{
		return new DefaultImporterSettings();
	}
}
