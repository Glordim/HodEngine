#pragma once

#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	class DefaultImporter : public Importer
	{
		REFLECTED_DERIVED_CLASS(DefaultImporter, Importer)

	public:

							DefaultImporter() = default;
							DefaultImporter(DefaultImporter&) = delete;
							DefaultImporter(DefaultImporter&&) = delete;
							~DefaultImporter() override = default;

		DefaultImporter&	operator = (DefaultImporter&) = delete;
		DefaultImporter&	operator = (DefaultImporter&&) = delete;

	protected:

		bool				WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource) override;
	};
}
