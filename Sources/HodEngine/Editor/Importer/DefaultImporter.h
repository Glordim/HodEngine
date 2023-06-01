#pragma once

#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	class DefaultImporterSettings : public ImporterSettings
	{
		REFLECTED_DERIVED_CLASS(DefaultImporterSettings, ImporterSettings)
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

		ImporterSettings*	AllocateSettings() override;
		const char*			GetTypeName() override;

	protected:

		bool				WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource, ImporterSettings& settings) override;
	};
}
