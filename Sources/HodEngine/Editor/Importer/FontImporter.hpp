#pragma once

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::editor
{
	class FontImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(FontImporterSettings, ImporterSettings)
	};

	class FontImporter : public Importer
	{
	public:

							FontImporter();
							FontImporter(const FontImporter&) = delete;
							FontImporter(FontImporter&&) = delete;
							~FontImporter() override = default;

		FontImporter&		operator = (const FontImporter&) = delete;
		FontImporter&		operator = (FontImporter&&) = delete;

	public:

		ImporterSettings*		AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
