#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontImporter.hpp"
#include "HodEngine/Editor/FontEditor/FontThumbnail.hpp"

#include <HodEngine/Text/FontFace.hpp>
#include <cstdint>

namespace hod::inline editor
{
	FontImporter::FontImporter()
	{
		SetAssetExtension("font");
		SetAssetType("Font");
	}

	bool FontImporter::FillDataBlock(Stream& source, ImporterSettings* /*importSettings*/)
	{
		Vector<uint8_t> ttf;
		ttf.Resize(source.GetSize());
		if (source.Read(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			OUTPUT_ERROR("FontImporter : Can't read Font data");
			return false;
		}

		Stream& ttfStream = AddDataBlockStream("ttf", true);
		if (ttfStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			OUTPUT_ERROR("FontImporter : Can't write Font data");
			return false;
		}

		// Drawn Smooth: the settings of a new asset. Made again with its settings when it is saved, see FontEditorTab
		FontFace        face;
		Vector<uint8_t> thumbnailPixels;
		if (face.LoadFromMemory(ttf.Data(), ttf.Size()) && FontThumbnail::Generate(face, FontResource::RenderMode::Smooth, 0, thumbnailPixels))
		{
			SetThumbnail(thumbnailPixels.Data(), FontThumbnail::Size, FontThumbnail::Size);
		}

		return true;
	}
}
