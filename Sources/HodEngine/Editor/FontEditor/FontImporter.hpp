#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API FontImporter : public Importer
	{
	public:

							FontImporter();
							FontImporter(const FontImporter&) = delete;
							FontImporter(FontImporter&&) = delete;
							~FontImporter() override = default;

		FontImporter&		operator = (const FontImporter&) = delete;
		FontImporter&		operator = (FontImporter&&) = delete;

	protected:

		bool FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
