#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API PrefabImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(PrefabImporterSettings, ImporterSettings, HOD_EDITOR_API)
	};

	class HOD_EDITOR_API PrefabImporter : public Importer
	{
	public:

							PrefabImporter() = default;
							PrefabImporter(PrefabImporter&) = delete;
							PrefabImporter(PrefabImporter&&) = delete;
							~PrefabImporter() override = default;

		PrefabImporter&		operator = (PrefabImporter&) = delete;
		PrefabImporter&		operator = (PrefabImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
