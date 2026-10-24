#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Texture::Texture()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Texture::~Texture()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	uint32_t Texture::GetWidth() const
	{
		return _width;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	uint32_t Texture::GetHeight() const
	{
		return _height;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	uint32_t Texture::GetMipLevelCount() const
	{
		return _mipLevelCount;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	uint32_t Texture::GetGeneration() const
	{
		return _generation;
	}

	/// @brief
	/// @param width
	/// @param height
	/// @return 1 + the index of the highest bit of the largest side, 0 for an empty texture
	uint32_t Texture::ComputeMipLevelCount(uint32_t width, uint32_t height)
	{
		uint32_t size = Max(width, height);
		uint32_t mipLevelCount = 0;
		while (size > 0)
		{
			++mipLevelCount;
			size >>= 1;
		}
		return mipLevelCount;
	}

	/// @brief
	/// @param size
	/// @param mipLevel
	/// @return
	uint32_t Texture::ComputeMipSize(uint32_t size, uint32_t mipLevel)
	{
		return Max(1u, size >> mipLevel);
	}

	/// @brief
	/// @param width
	/// @param height
	/// @param mipLevelCount
	/// @return
	uint32_t Texture::ComputeMipChainByteSize(uint32_t width, uint32_t height, uint32_t mipLevelCount, PixelFormat format)
	{
		const uint32_t pixelByteSize = GetPixelByteSize(format);

		uint32_t byteSize = 0;
		for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
		{
			byteSize += ComputeMipSize(width, mipLevel) * ComputeMipSize(height, mipLevel) * pixelByteSize;
		}
		return byteSize;
	}

	/// @brief
	/// @param format
	/// @return
	uint32_t Texture::GetPixelByteSize(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::R8G8B8A8_UNorm: return 4;
			case PixelFormat::D16_UNorm: return 2;
			case PixelFormat::R8_UNorm: return 1;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	PixelFormat Texture::GetFormat() const
	{
		return _format;
	}

	/// @brief
	/// @param x
	/// @param y
	/// @param width
	/// @param height
	/// @param pixels
	/// @param mipLevel
	/// @return
	bool Texture::UpdateRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel)
	{
		if (_builtFromBuffer == false)
		{
			OUTPUT_ERROR("Texture::UpdateRegion: only for a texture built with BuildBuffer");
			return false;
		}

		if (pixels == nullptr || width == 0 || height == 0)
		{
			OUTPUT_ERROR("Texture::UpdateRegion: no pixels");
			return false;
		}

		if (mipLevel >= _mipLevelCount)
		{
			OUTPUT_ERROR("Texture::UpdateRegion: mip level {} out of the {} of the texture", mipLevel, _mipLevelCount);
			return false;
		}

		const uint32_t mipWidth = ComputeMipSize(_width, mipLevel);
		const uint32_t mipHeight = ComputeMipSize(_height, mipLevel);
		if (x > mipWidth || width > mipWidth - x || y > mipHeight || height > mipHeight - y)
		{
			OUTPUT_ERROR("Texture::UpdateRegion: region ({}, {}) {}x{} out of the {}x{} level", x, y, width, height, mipWidth, mipHeight);
			return false;
		}

		return UploadRegion(x, y, width, height, pixels, mipLevel);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void Texture::SetDefaultSampler(const Sampler* sampler)
	{
		_defaultSampler = sampler;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	const Sampler* Texture::GetDefaultSampler() const
	{
		return _defaultSampler;
	}
}
