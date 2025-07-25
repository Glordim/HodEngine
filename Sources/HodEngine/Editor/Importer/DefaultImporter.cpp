#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/DefaultImporter.hpp"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(DefaultImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool DefaultImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		return false;
	}

	/// @brief 
	/// @return 
	const char* DefaultImporter::GetTypeName() const
	{
		return "DefaultImporter";
	}

	std::shared_ptr<ImporterSettings> DefaultImporter::AllocateSettings() const
	{
		return std::make_shared<DefaultImporterSettings>();
	}

	ReflectionDescriptor* DefaultImporter::GetResourceDescriptor() const
	{
		return nullptr;
	}
}
