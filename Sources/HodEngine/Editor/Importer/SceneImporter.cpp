#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(SceneImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool SceneImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
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
