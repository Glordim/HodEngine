#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <cstdint>
#include <cstddef>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/RHI/Enums.hpp"
#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline rhi
{
	class CommandBuffer;
	class Sampler;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API Texture
	{
	public:

		struct CreateInfo
		{
			bool		_allowReadWrite = false;
			uint32_t	_mipLevelCount = 1; // BuildBuffer only, at most ComputeMipLevelCount(width, height)
			PixelFormat	_format = PixelFormat::R8G8B8A8_UNorm; // BuildBuffer only: R8G8B8A8_UNorm or R8_UNorm
		};

		// The number of levels of a full mip chain: down to 1 x 1
		static uint32_t	ComputeMipLevelCount(uint32_t width, uint32_t height);
		// The size of a side of a level of the mip chain, at least 1
		static uint32_t	ComputeMipSize(uint32_t size, uint32_t mipLevel);
		// The size, in bytes, of the mip chain of a texture, the levels one after the other
		static uint32_t	ComputeMipChainByteSize(uint32_t width, uint32_t height, uint32_t mipLevelCount, PixelFormat format = PixelFormat::R8G8B8A8_UNorm);
		// The size, in bytes, of a pixel. 0 for PixelFormat::None
		static uint32_t	GetPixelByteSize(PixelFormat format);

	public:

		Texture();
		virtual ~Texture();

		uint32_t	GetWidth() const;
		uint32_t	GetHeight() const;
		uint32_t	GetMipLevelCount() const;
		PixelFormat	GetFormat() const;

		// Changes each time the texture is built: what was made with the previous build (BindGroups...) is of no use anymore
		uint32_t	GetGeneration() const;

		// The sampler to read the texture with when nothing asks for another one.
		// None by default, which leaves the choice to RhiDevice::GetSampler(Sampler::CreateInfo()). Can be changed at any time.
		void			SetDefaultSampler(const Sampler* sampler);
		const Sampler*	GetDefaultSampler() const;

		virtual bool BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) = 0;
		virtual bool BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) = 0;
		// buffer: pixels, in CreateInfo::_format, of the CreateInfo::_mipLevelCount levels, one after the other, tightly packed (see ComputeMipChainByteSize).
		// Null to leave the content undefined, to be filled with UpdateRegion.
		// Can be called again on a texture already built: replaces it (live edit of a texture)
		virtual bool BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) = 0;

		// Replaces a region of a level of a texture built with BuildBuffer (a glyph in a font atlas...), the rest is kept.
		// pixels: width x height pixels in the texture's format, tightly packed rows.
		// Like BuildBuffer, the copy is done ahead of what is submitted next, without the CPU waiting for it:
		// the draws already recorded but not submitted yet see the new content too.
		bool UpdateRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel = 0);

		virtual Color ReadPixel(const Vector2& position) const = 0;

		// Records (into the given, already-recording command buffer) whatever is needed to capture
		// this texture's current content for a later ReadPixel() call. No-op for textures that were
		// not built with CreateInfo::_allowReadWrite.
		virtual void CaptureReadback(CommandBuffer* commandBuffer) = 0;

	protected:

		// UpdateRegion, once the arguments are checked: the region is within the level, of a texture built with BuildBuffer
		virtual bool UploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel) = 0;

	protected:

		uint32_t	_width = 0;
		uint32_t	_height = 0;
		uint32_t	_mipLevelCount = 1;
		PixelFormat	_format = PixelFormat::None; // set by the backends on each successful build
		bool		_builtFromBuffer = false; // set by the backends on each successful build: UpdateRegion is allowed
		uint32_t	_generation = 0; // incremented by the backends on each successful build

		const Sampler*	_defaultSampler = nullptr;
	};
}
