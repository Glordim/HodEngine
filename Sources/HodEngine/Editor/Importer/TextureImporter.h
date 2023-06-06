#pragma once

#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	class TextureImporterSettings : public ImporterSettings
	{
		REFLECTED_DERIVED_CLASS(TextureImporterSettings, ImporterSettings)

	public:

		bool				_generateMipmap = false;
	};

	class TextureImporter : public Importer
	{
	public:

							TextureImporter();
							TextureImporter(const TextureImporter&) = delete;
							TextureImporter(TextureImporter&&) = delete;
							~TextureImporter() override = default;

		TextureImporter&	operator = (const TextureImporter&) = delete;
		TextureImporter&	operator = (TextureImporter&&) = delete;

	public:

		ImporterSettings*	AllocateSettings() override;
		const char*			GetTypeName() override;

	protected:

		bool				WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource, core::FileStream& thumbnail, ImporterSettings& settings) override;
	};
}
