#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API TextureImporter : public Importer
	{
	public:
		TextureImporter();
		TextureImporter(const TextureImporter&) = delete;
		TextureImporter(TextureImporter&&) = delete;
		~TextureImporter() override = default;

		TextureImporter& operator=(const TextureImporter&) = delete;
		TextureImporter& operator=(TextureImporter&&) = delete;

	protected:

		bool FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
