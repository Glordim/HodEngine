#pragma once

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class TextureImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(TextureImporterSettings, ImporterSettings)

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

		ImporterSettings*	AllocateSettings() const override;
		const char*			GetTypeName() const override;

	protected:

		bool				WriteResource(FileStream& data, FileStream& meta, FileStream& resource, FileStream& thumbnail, ImporterSettings& settings) override;
	};
}