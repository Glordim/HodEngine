#pragma once

#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	class DefaultImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(DefaultImporterSettings)
	};

	class DefaultImporter : public Importer
	{
	public:

							DefaultImporter() = default;
							DefaultImporter(DefaultImporter&) = delete;
							DefaultImporter(DefaultImporter&&) = delete;
							~DefaultImporter() override = default;

		DefaultImporter&	operator = (DefaultImporter&) = delete;
		DefaultImporter&	operator = (DefaultImporter&&) = delete;

	public:

		ImporterSettings*	AllocateSettings() const override;
		const char*			GetTypeName() const override;

	protected:

		bool				WriteResource(FileStream& data, FileStream& meta, FileStream& resource, FileStream& thumbnail, ImporterSettings& settings) override;
	};
}
