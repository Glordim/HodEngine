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
	PixelFormat Texture::GetFormat() const
	{
		return _format;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	TextureUsage Texture::GetUsage() const
	{
		return _usage;
	}

	/// @brief
	/// @param usage
	/// @return
	bool Texture::HasUsage(TextureUsage usage) const
	{
		return (_usage & usage) == usage;
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

	/// @brief
	/// @param format
	/// @return
	bool Texture::IsDepthFormat(PixelFormat format)
	{
		return format == PixelFormat::D16_UNorm;
	}

	/// @brief The combinations of TextureUsage the backends build, see TextureUsage
	/// @param createInfo
	/// @param pixels
	/// @return
	bool Texture::CheckCreateInfo(const CreateInfo& createInfo, const uint8_t* pixels) const
	{
		if (createInfo._width == 0 || createInfo._height == 0)
		{
			OUTPUT_ERROR("Texture::Build: empty texture ({}x{})", createInfo._width, createInfo._height);
			return false;
		}

		const TextureUsage usage = createInfo._usage;
		if (usage == (TextureUsage::Sampled | TextureUsage::CopyDst))
		{
			if (createInfo._format != PixelFormat::R8G8B8A8_UNorm && createInfo._format != PixelFormat::R8_UNorm)
			{
				OUTPUT_ERROR("Texture::Build: a CopyDst texture is R8G8B8A8_UNorm or R8_UNorm");
				return false;
			}
		}
		else if (usage == (TextureUsage::Sampled | TextureUsage::RenderTarget) || usage == (TextureUsage::Sampled | TextureUsage::RenderTarget | TextureUsage::Readback))
		{
			if (createInfo._format != PixelFormat::R8G8B8A8_UNorm)
			{
				OUTPUT_ERROR("Texture::Build: a color RenderTarget texture is R8G8B8A8_UNorm");
				return false;
			}
		}
		else if (usage == TextureUsage::RenderTarget)
		{
			if (IsDepthFormat(createInfo._format) == false)
			{
				OUTPUT_ERROR("Texture::Build: a RenderTarget texture that is not Sampled is a depth one");
				return false;
			}
		}
		else
		{
			OUTPUT_ERROR("Texture::Build: unsupported usage {}, see TextureUsage", std::to_underlying(usage));
			return false;
		}

		if (pixels != nullptr && (usage & TextureUsage::CopyDst) == TextureUsage::None)
		{
			OUTPUT_ERROR("Texture::Build: pixels for a texture that is not CopyDst");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param createInfo
	/// @param pixels
	/// @return
	bool Texture::Build(const CreateInfo& createInfo, const uint8_t* pixels)
	{
		if (CheckCreateInfo(createInfo, pixels) == false)
		{
			return false;
		}

		CreateInfo checkedCreateInfo = createInfo;
		if ((createInfo._usage & TextureUsage::CopyDst) == TextureUsage::CopyDst)
		{
			checkedCreateInfo._mipLevelCount = Clamp(createInfo._mipLevelCount, 1u, ComputeMipLevelCount(createInfo._width, createInfo._height));
		}
		else
		{
			checkedCreateInfo._mipLevelCount = 1;
		}

		if (BuildResource(checkedCreateInfo, pixels) == false)
		{
			// What was built is released all the same
			_width = 0;
			_height = 0;
			_mipLevelCount = 1;
			_format = PixelFormat::None;
			_usage = TextureUsage::None;
			++_generation;
			return false;
		}

		_width = checkedCreateInfo._width;
		_height = checkedCreateInfo._height;
		_mipLevelCount = checkedCreateInfo._mipLevelCount;
		_format = checkedCreateInfo._format;
		_usage = checkedCreateInfo._usage;
		++_generation;
		return true;
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
		if (HasUsage(TextureUsage::CopyDst) == false)
		{
			OUTPUT_ERROR("Texture::UpdateRegion: only for a CopyDst texture");
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
