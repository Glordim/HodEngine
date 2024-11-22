#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API ShaderImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(ShaderImporterSettings, ImporterSettings)
	};

	class HOD_EDITOR_API ShaderImporter : public Importer
	{
	public:

							ShaderImporter();
							ShaderImporter(const ShaderImporter&) = delete;
							ShaderImporter(ShaderImporter&&) = delete;
							~ShaderImporter() override = default;

		ShaderImporter&		operator = (const ShaderImporter&) = delete;
		ShaderImporter&		operator = (ShaderImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
