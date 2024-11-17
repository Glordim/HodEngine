#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API SerializedDataImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(SerializedDataImporterSettings, ImporterSettings)
	};

	class HOD_EDITOR_API SerializedDataImporter : public Importer
	{
	public:

									SerializedDataImporter() = default;
									SerializedDataImporter(SerializedDataImporter&) = delete;
									SerializedDataImporter(SerializedDataImporter&&) = delete;
									~SerializedDataImporter() override = default;

		SerializedDataImporter&		operator = (SerializedDataImporter&) = delete;
		SerializedDataImporter&		operator = (SerializedDataImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
