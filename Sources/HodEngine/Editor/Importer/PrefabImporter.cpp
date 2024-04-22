#include "HodEngine/Editor/Importer/PrefabImporter.hpp"

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
		return false;
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
		return nullptr;
	}
}
