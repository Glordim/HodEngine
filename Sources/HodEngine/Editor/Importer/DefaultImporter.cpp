#include "HodEngine/Editor/Importer/DefaultImporter.hpp"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(DefaultImporterSettings, ImporterSettings)
	{

	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
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

	ReflectionDescriptor* DefaultImporter::GetResourceDescriptor() const
	{
		return nullptr;
	}
}
