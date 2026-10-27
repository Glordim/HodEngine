#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API FontFaceImporter : public Importer
	{
	public:

							FontFaceImporter();
							FontFaceImporter(const FontFaceImporter&) = delete;
							FontFaceImporter(FontFaceImporter&&) = delete;
							~FontFaceImporter() override = default;

		FontFaceImporter&		operator = (const FontFaceImporter&) = delete;
		FontFaceImporter&		operator = (FontFaceImporter&&) = delete;

	protected:

		bool FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
