#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API DefaultImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(DefaultImporterSettings, ImporterSettings)
	};

	class HOD_EDITOR_API DefaultImporter : public Importer
	{
	public:

							DefaultImporter() = default;
							DefaultImporter(DefaultImporter&) = delete;
							DefaultImporter(DefaultImporter&&) = delete;
							~DefaultImporter() override = default;

		DefaultImporter&	operator = (DefaultImporter&) = delete;
		DefaultImporter&	operator = (DefaultImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
