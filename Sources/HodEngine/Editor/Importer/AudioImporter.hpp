#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Math/Rect.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Renderer/Enums.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API AudioImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(AudioImporterSettings, ImporterSettings)

	public:
		bool _loop = false;
	};

	class HOD_EDITOR_API AudioImporter : public Importer
	{
	public:
		AudioImporter();
		AudioImporter(const AudioImporter&) = delete;
		AudioImporter(AudioImporter&&) = delete;
		~AudioImporter() override = default;

		AudioImporter& operator=(const AudioImporter&) = delete;
		AudioImporter& operator=(AudioImporter&&) = delete;

	public:
		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*                       GetTypeName() const override;
		ReflectionDescriptor*             GetResourceDescriptor() const override;

	protected:
		bool WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail,
		                   ImporterSettings& settings) override;
	};
}
