#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Core/Math.hpp>

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
	uint32_t Texture::ComputeMipChainByteSize(uint32_t width, uint32_t height, uint32_t mipLevelCount)
	{
		uint32_t byteSize = 0;
		for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
		{
			byteSize += ComputeMipSize(width, mipLevel) * ComputeMipSize(height, mipLevel) * 4;
		}
		return byteSize;
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
