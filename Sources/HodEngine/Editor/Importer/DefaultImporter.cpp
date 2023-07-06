#include "HodEngine/Editor/Importer/DefaultImporter.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(DefaultImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource, core::FileStream& thumbnail, ImporterSettings& settings)
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
