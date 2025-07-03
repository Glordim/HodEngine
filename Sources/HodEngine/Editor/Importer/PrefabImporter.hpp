#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API PrefabImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(PrefabImporterSettings, ImporterSettings)
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

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
