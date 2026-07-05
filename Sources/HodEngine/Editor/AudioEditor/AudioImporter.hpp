#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API AudioImporter : public Importer
	{
	public:
		AudioImporter() = default;
		AudioImporter(const AudioImporter&) = delete;
		AudioImporter(AudioImporter&&) = delete;
		~AudioImporter() override = default;

		AudioImporter& operator=(const AudioImporter&) = delete;
		AudioImporter& operator=(AudioImporter&&) = delete;

	protected:
		bool FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
