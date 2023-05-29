#pragma once

#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	class TextureImporter : public Importer
	{
		REFLECTED_DERIVED_CLASS(TextureImporter, Importer)

	public:

							TextureImporter();
							TextureImporter(const TextureImporter&) = delete;
							TextureImporter(TextureImporter&&) = delete;
							~TextureImporter() override = default;

		TextureImporter&	operator = (const TextureImporter&) = delete;
		TextureImporter&	operator = (TextureImporter&&) = delete;

	protected:

		bool				WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource) override;

	private:

		bool				_generateMipmap = false;
	};
}
