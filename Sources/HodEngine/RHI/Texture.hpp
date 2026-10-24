#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <cstdint>
#include <cstddef>
#include <utility>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/RHI/Enums.hpp"
#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline rhi
{
	class CommandBuffer;
	class Sampler;

	/// @brief What a texture is built for, combined with |. Texture::Build takes:
	/// - Sampled | CopyDst: a texture filled by the CPU (pixels given to Build, UpdateRegion), R8G8B8A8_UNorm or R8_UNorm
	/// - Sampled | RenderTarget, plus Readback to read it on the CPU: a color texture drawn into, R8G8B8A8_UNorm
	/// - RenderTarget: a depth texture drawn into, D16_UNorm
	enum class TextureUsage : uint8_t
	{
		None = 0,
		Sampled = 1 << 0,      // read by the shaders
		CopyDst = 1 << 1,      // filled by the CPU: the pixels given to Build, UpdateRegion
		RenderTarget = 1 << 2, // drawn into, as the color or the depth of a RenderTarget, after its format
		Readback = 1 << 3,     // read by the CPU: CaptureReadback, ReadPixel
	};

	constexpr TextureUsage operator|(TextureUsage left, TextureUsage right)
	{
		return static_cast<TextureUsage>(std::to_underlying(left) | std::to_underlying(right));
	}

	constexpr TextureUsage operator&(TextureUsage left, TextureUsage right)
	{
		return static_cast<TextureUsage>(std::to_underlying(left) & std::to_underlying(right));
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API Texture
	{
	public:

		struct CreateInfo
		{
			uint32_t		_width = 0;
			uint32_t		_height = 0;
			PixelFormat		_format = PixelFormat::R8G8B8A8_UNorm;
			TextureUsage	_usage = TextureUsage::Sampled | TextureUsage::CopyDst;
			uint32_t		_mipLevelCount = 1; // CopyDst only, at most ComputeMipLevelCount(width, height)
		};

		// The number of levels of a full mip chain: down to 1 x 1
		static uint32_t	ComputeMipLevelCount(uint32_t width, uint32_t height);
		// The size of a side of a level of the mip chain, at least 1
		static uint32_t	ComputeMipSize(uint32_t size, uint32_t mipLevel);
		// The size, in bytes, of the mip chain of a texture, the levels one after the other
		static uint32_t	ComputeMipChainByteSize(uint32_t width, uint32_t height, uint32_t mipLevelCount, PixelFormat format = PixelFormat::R8G8B8A8_UNorm);
		// The size, in bytes, of a pixel. 0 for PixelFormat::None
		static uint32_t	GetPixelByteSize(PixelFormat format);
		static bool		IsDepthFormat(PixelFormat format);

	public:

		Texture();
		virtual ~Texture();

		uint32_t		GetWidth() const;
		uint32_t		GetHeight() const;
		uint32_t		GetMipLevelCount() const;
		PixelFormat		GetFormat() const;
		TextureUsage	GetUsage() const;
		bool			HasUsage(TextureUsage usage) const; // all of the given ones

		// Changes each time the texture is built: what was made with the previous build (BindGroups...) is of no use anymore
		uint32_t	GetGeneration() const;

		// The sampler to read the texture with when nothing asks for another one.
		// None by default, which leaves the choice to RhiDevice::GetSampler(Sampler::CreateInfo()). Can be changed at any time.
		void			SetDefaultSampler(const Sampler* sampler);
		const Sampler*	GetDefaultSampler() const;

		// pixels: CopyDst only, in CreateInfo::_format, of the CreateInfo::_mipLevelCount levels, one after the other, tightly packed (see ComputeMipChainByteSize).
		// Null to leave the content undefined, to be filled with UpdateRegion.
		// Can be called again on a texture already built: replaces it (live edit of a texture), what was built is kept until the GPU is done with it.
		// Not built for a CreateInfo the TextureUsage combinations do not allow: the texture is kept as it was.
		bool Build(const CreateInfo& createInfo, const uint8_t* pixels = nullptr);

		// Replaces a region of a level of a CopyDst texture (a glyph in a font atlas...), the rest is kept.
		// pixels: width x height pixels in the texture's format, tightly packed rows.
		// Like Build, the copy is done ahead of what is submitted next, without the CPU waiting for it:
		// the draws already recorded but not submitted yet see the new content too.
		bool UpdateRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel = 0);

		// Readback textures only, Color(0, 0, 0, 0) otherwise or before a frame captured it
		virtual Color ReadPixel(const Vector2& position) const = 0;

		// Records (into the given, already-recording command buffer) whatever is needed to capture
		// this texture's current content for a later ReadPixel() call. No-op for textures that are not Readback ones.
		virtual void CaptureReadback(CommandBuffer* commandBuffer) = 0;

	protected:

		// Build, once the CreateInfo is checked (_mipLevelCount within the chain, 1 for a RenderTarget).
		// Replaces what was built, nothing is left on failure. The base class keeps the size, format, usage and generation.
		virtual bool BuildResource(const CreateInfo& createInfo, const uint8_t* pixels) = 0;

		// UpdateRegion, once the arguments are checked: the region is within the level, of a CopyDst texture
		virtual bool UploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel) = 0;

	private:

		bool		CheckCreateInfo(const CreateInfo& createInfo, const uint8_t* pixels) const;

	private:

		uint32_t		_width = 0;
		uint32_t		_height = 0;
		uint32_t		_mipLevelCount = 1;
		PixelFormat		_format = PixelFormat::None;
		TextureUsage	_usage = TextureUsage::None;
		uint32_t		_generation = 0; // incremented on each successful build

		const Sampler*	_defaultSampler = nullptr;
	};
}
