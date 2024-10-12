#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API FontImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(FontImporterSettings, ImporterSettings, HOD_EDITOR_API)
	};

	class HOD_EDITOR_API FontImporter : public Importer
	{
	public:

							FontImporter();
							FontImporter(const FontImporter&) = delete;
							FontImporter(FontImporter&&) = delete;
							~FontImporter() override = default;

		FontImporter&		operator = (const FontImporter&) = delete;
		FontImporter&		operator = (FontImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
