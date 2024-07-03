#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_API PrefabImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(PrefabImporterSettings, ImporterSettings)
	};

	class HOD_API PrefabImporter : public Importer
	{
	public:

							PrefabImporter() = default;
							PrefabImporter(PrefabImporter&) = delete;
							PrefabImporter(PrefabImporter&&) = delete;
							~PrefabImporter() override = default;

		PrefabImporter&		operator = (PrefabImporter&) = delete;
		PrefabImporter&		operator = (PrefabImporter&&) = delete;

	public:

		ImporterSettings*		AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
