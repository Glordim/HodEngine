#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/RendererEditor/FontEditor/FontCooker.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/RHI/Enums.hpp>

#include <memory>

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline editor
{
	class Asset;

	/// @brief What the font and font face editors share: the style a face says it is, the family of a face in its folder, a font made from faces
	class HOD_RENDERER_EDITOR_API FontHelper
	{
	public:

		// The weight and the italic a face says it has (OS/2), the closest of the weights: a face may say 350, or nothing (0, Regular then). Its face not set
		static FontSettings::Style			DetectStyle(const FontFace& face);

		// Adds the face, then the faces of its folder (not below it) whose face has its family and its width ("Noto Sans Bold" for "Noto Sans",
		// not "Noto Sans Condensed Bold"), each for the style its file says, sorted. But for the ones already in, and the ones of a style already
		// taken: said in the message, with what was added. The number added
		static uint32_t						DetectStyles(FontSettings& fontSettings, const std::shared_ptr<Asset>& faceAsset, String& message);

		// A font in the directory, named after the family of the first face, its styles detected from each face (see DetectStyles). Empty when not made
		static Path							CreateFont(const Vector<std::shared_ptr<Asset>>& faceAssets, const Path& directory);

		// The fonts one of the styles of is the face, as they are cooked
		static Vector<std::shared_ptr<Asset>>	FindFontsUsing(const UID& face);

		// The thumbnail of a font: the one of its face for Regular, as it is drawn. False without one
		static bool							GenerateThumbnail(const FontSettings& fontSettings, Vector<uint8_t>& thumbnail, FilterMode& filterMode);
	};
}
