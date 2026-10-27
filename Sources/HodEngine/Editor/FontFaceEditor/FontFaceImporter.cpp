#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceImporter.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontThumbnail.hpp"

#include <HodEngine/Text/FontFace.hpp>
#include <cstdint>

namespace hod::inline editor
{
	FontFaceImporter::FontFaceImporter()
	{
		SetAssetExtension("fontface");
		SetAssetType("FontFace");
	}

	bool FontFaceImporter::FillDataBlock(Stream& source, ImporterSettings* /*importSettings*/)
	{
		Vector<uint8_t> ttf;
		ttf.Resize(source.GetSize());
		if (source.Read(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			OUTPUT_ERROR("FontFaceImporter : Can't read Font data");
			return false;
		}

		Stream& ttfStream = AddDataBlockStream("ttf", true);
		if (ttfStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			OUTPUT_ERROR("FontFaceImporter : Can't write Font data");
			return false;
		}

		// Drawn Smooth: the settings of a new asset. Made again with its settings when it is saved, see FontFaceEditorTab
		FontFace        face;
		Vector<uint8_t> thumbnailPixels;
		if (face.LoadFromMemory(ttf.Data(), ttf.Size()) && FontThumbnail::Generate(face, FontFaceResource::RenderMode::Smooth, 0, thumbnailPixels))
		{
			SetThumbnail(thumbnailPixels.Data(), FontThumbnail::Size, FontThumbnail::Size);
		}

		return true;
	}
}
