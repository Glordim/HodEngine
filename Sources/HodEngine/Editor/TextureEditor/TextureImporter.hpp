#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API TextureImporter : public Importer
	{
	public:
		TextureImporter();
		TextureImporter(const TextureImporter&) = delete;
		TextureImporter(TextureImporter&&) = delete;
		~TextureImporter() override = default;

		TextureImporter& operator=(const TextureImporter&) = delete;
		TextureImporter& operator=(TextureImporter&&) = delete;

	public:
		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*                       GetTypeName() const override;
		ReflectionDescriptor*             GetResourceDescriptor() const override;

	protected:
		bool WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail,
		                   ImporterSettings& settings) override;

		bool FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
