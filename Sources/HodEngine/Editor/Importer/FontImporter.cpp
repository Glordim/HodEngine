#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/FontImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
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
	bool FontImporter::WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail, ImporterSettings& settings)
	{
		(void)document;
		(void)meta;
		(void)thumbnail;
		(void)settings;

		Resource::Data ttfData;
		ttfData._size = data.GetSize();
		ttfData._buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfData._size);
		if (data.Read(ttfData._buffer, ttfData._size) != ttfData._size)
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
		return &FontResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> FontImporter::AllocateSettings() const
	{
		return std::make_shared<FontImporterSettings>();
	}
}
