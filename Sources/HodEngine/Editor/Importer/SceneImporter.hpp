#pragma once

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class SceneImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(SceneImporterSettings, ImporterSettings)
	};

	class SceneImporter : public Importer
	{
	public:

							SceneImporter() = default;
							SceneImporter(SceneImporter&) = delete;
							SceneImporter(SceneImporter&&) = delete;
							~SceneImporter() override = default;

		SceneImporter&		operator = (SceneImporter&) = delete;
		SceneImporter&		operator = (SceneImporter&&) = delete;

	public:

		ImporterSettings*		AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
