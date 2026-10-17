#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/RHI/Enums.hpp>

#include <cstdint>

namespace hod::inline rhi
{
	class Texture;
}

namespace hod::inline editor
{
	/// @brief Editor-only asset preview, stored in the .asset Thumbnail block (see AssetContainer) as
	/// [uint16 width][uint16 height][compressed RGB8 pixels]. Always opaque: transparency is flattened
	/// over a checker at encode time, so displaying it needs no extra draw or blending.
	class HOD_EDITOR_API Thumbnail
	{
	public:
		/// @brief Largest side of a generated thumbnail, in pixels
		static constexpr uint32_t MaxSize = 256;

	public:
		/// @brief Downscale (keeping aspect ratio, never upscaling), flatten over a checker, then encode RGBA8 pixels
		static bool Encode(const uint8_t* rgbaPixels, uint32_t width, uint32_t height, Vector<uint8_t>& encoded);

		/// @brief Decode an encoded thumbnail into a new RGBA Texture, owned by the caller (nullptr on failure)
		/// @param filterMode Filter of the texture default Sampler, used when the thumbnail is displayed bigger than it is
		static Texture* CreateTexture(const Vector<uint8_t>& encoded, FilterMode filterMode);
	};
}
