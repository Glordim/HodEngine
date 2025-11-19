#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/PrefabImporter.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(PrefabImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool PrefabImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		// TODO
		(void)meta;
		(void)datas;
		(void)thumbnail;
		(void)settings;

		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
		{
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	const char* PrefabImporter::GetTypeName() const
	{
		return "PrefabImporter";
	}

	std::shared_ptr<ImporterSettings> PrefabImporter::AllocateSettings() const
	{
		return std::make_shared<PrefabImporterSettings>();
	}

	ReflectionDescriptor* PrefabImporter::GetResourceDescriptor() const
	{
		return &game::PrefabResource::GetReflectionDescriptor();
	}
}
