#include "HodEngine/Editor/Importer/DefaultImporter.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_DERIVED_CLASS(DefaultImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource, ImporterSettings& settings)
	{
		return false;
	}

	/// @brief 
	/// @return 
	const char* DefaultImporter::GetTypeName()
	{
		return "DefaultImporter";
	}

	ImporterSettings* DefaultImporter::AllocateSettings()
	{
		return new DefaultImporterSettings();
	}
}
