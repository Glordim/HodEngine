#include "HodEngine/Editor/Importer/DefaultImporter.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_DERIVED_CLASS(DefaultImporter, Importer)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource)
	{
		return false;
	}
}
