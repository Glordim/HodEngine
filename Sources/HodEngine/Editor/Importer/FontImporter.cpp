#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/FontImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(FontImporterSettings, reflectionDescriptor)
	{
	}

	/// @brief 
	FontImporter::FontImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("ttf");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool FontImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		Resource::Data ttfData;
		ttfData._size = FileSystem::GetInstance()->GetSize(data);
		ttfData._buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfData._size);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(ttfData._buffer), ttfData._size) != ttfData._size)
		{
			OUTPUT_ERROR("FontImporter : Can't read Font data");
			return false;
		}
		datas.push_back(ttfData);
		return true;
	}

	// TODO Move all virtual in Ctor const init ?

	/// @brief 
	/// @return 
	const char* FontImporter::GetTypeName() const
	{
		return "FontImporter";
	}

	ReflectionDescriptor* FontImporter::GetResourceDescriptor() const
	{
		return &renderer::FontResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> FontImporter::AllocateSettings() const
	{
		return std::make_shared<FontImporterSettings>();
	}
}
