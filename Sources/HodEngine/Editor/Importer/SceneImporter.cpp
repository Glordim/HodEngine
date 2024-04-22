#include "HodEngine/Editor/Importer/SceneImporter.hpp"

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
		return false;
	}

	/// @brief 
	/// @return 
	const char* SceneImporter::GetTypeName() const
	{
		return "SceneImporter";
	}

	ImporterSettings* SceneImporter::AllocateSettings() const
	{
		return new SceneImporterSettings();
	}

	ReflectionDescriptor* SceneImporter::GetResourceDescriptor() const
	{
		return nullptr;
	}
}
