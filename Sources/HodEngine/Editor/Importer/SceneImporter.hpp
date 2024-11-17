#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API SceneImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(SceneImporterSettings, ImporterSettings)
	};

	class HOD_EDITOR_API SceneImporter : public Importer
	{
	public:

							SceneImporter() = default;
							SceneImporter(SceneImporter&) = delete;
							SceneImporter(SceneImporter&&) = delete;
							~SceneImporter() override = default;

		SceneImporter&		operator = (SceneImporter&) = delete;
		SceneImporter&		operator = (SceneImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
