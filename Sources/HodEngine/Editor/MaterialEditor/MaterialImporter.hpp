#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialImporter : public Importer
	{
	public:

							MaterialImporter();
							MaterialImporter(const MaterialImporter&) = delete;
							MaterialImporter(MaterialImporter&&) = delete;
							~MaterialImporter() override = default;

		MaterialImporter&	operator = (const MaterialImporter&) = delete;
		MaterialImporter&	operator = (MaterialImporter&&) = delete;

	protected:

		bool	FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
