#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"
#include "HodEngine/Game/SerializedDataResource.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(SerializedDataImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool SerializedDataImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
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
	const char* SerializedDataImporter::GetTypeName() const
	{
		return "SerializedDataImporter";
	}

	std::shared_ptr<ImporterSettings> SerializedDataImporter::AllocateSettings() const
	{
		return std::make_shared<SerializedDataImporterSettings>();
	}

	ReflectionDescriptor* SerializedDataImporter::GetResourceDescriptor() const
	{
		return &game::SerializedDataResource::GetReflectionDescriptor();
	}
}
