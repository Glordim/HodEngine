#pragma once

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class DefaultImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(DefaultImporterSettings, ImporterSettings)
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

		ImporterSettings*		AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
