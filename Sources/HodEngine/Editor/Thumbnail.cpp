#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include "HodEngine/Core/Math.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/MemoryStream.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/Sampler.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include <cstring>

#include <stb_image_resize2.h>

namespace hod::inline editor
{
	static constexpr uint32_t SourceComponentCount = 4; // RGBA8
	static constexpr uint32_t StoredComponentCount = 3; // RGB8, opaque
	static constexpr uint32_t TextureComponentCount = 4; // RGBA8, what Texture::BuildBuffer() expects

	static constexpr uint32_t CheckerSquareCount = 16; // Along the largest side, whatever the thumbnail size

	/// @brief
	/// @param rgbaPixels
	/// @param width
	/// @param height
	/// @param encoded
	/// @return
	bool Thumbnail::Encode(const uint8_t* rgbaPixels, uint32_t width, uint32_t height, Vector<uint8_t>& encoded)
	{
		encoded.Clear();

		if (rgbaPixels == nullptr || width == 0 || height == 0)
		{
			OUTPUT_ERROR("Thumbnail::Encode: invalid source image");
			return false;
		}

		uint16_t thumbnailWidth = (uint16_t)Min(width, MaxSize);
		uint16_t thumbnailHeight = (uint16_t)Min(height, MaxSize);
		if (width > MaxSize || height > MaxSize)
		{
			if (width >= height)
			{
				thumbnailHeight = (uint16_t)Max(1u, (uint32_t)(((float)height / (float)width) * MaxSize));
			}
			else
			{
				thumbnailWidth = (uint16_t)Max(1u, (uint32_t)(((float)width / (float)height) * MaxSize));
			}
		}

		const uint8_t* thumbnailPixels = rgbaPixels;
		uint8_t*       resizedPixels = nullptr;
		if (thumbnailWidth != width || thumbnailHeight != height)
		{
			resizedPixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(thumbnailWidth * thumbnailHeight * SourceComponentCount);
			if (stbir_resize_uint8_linear(rgbaPixels, (int)width, (int)height, 0, resizedPixels, (int)thumbnailWidth, (int)thumbnailHeight, 0, STBIR_RGBA) == nullptr)
			{
				OUTPUT_ERROR("Thumbnail::Encode: resize failed");
				DefaultAllocator::GetInstance().Free(resizedPixels);
				return false;
			}
			thumbnailPixels = resizedPixels;
		}

		// Flatten over a checker so transparency stays visible without any extra draw at display time
		uint32_t checkerSquareSize = Max(1u, (uint32_t)Max(thumbnailWidth, thumbnailHeight) / CheckerSquareCount);
		uint32_t rgbPixelsSize = thumbnailWidth * thumbnailHeight * StoredComponentCount;
		uint8_t* rgbPixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(rgbPixelsSize);
		for (uint32_t y = 0; y < thumbnailHeight; ++y)
		{
			for (uint32_t x = 0; x < thumbnailWidth; ++x)
			{
				const uint8_t* source = thumbnailPixels + (y * thumbnailWidth + x) * SourceComponentCount;
				uint8_t*       destination = rgbPixels + (y * thumbnailWidth + x) * StoredComponentCount;

				uint32_t background = (((x / checkerSquareSize) + (y / checkerSquareSize)) % 2 == 0) ? Editor::CheckerPrimaryGrey : Editor::CheckerSecondaryGrey;
				uint32_t alpha = source[3];
				for (uint32_t channel = 0; channel < StoredComponentCount; ++channel)
				{
					destination[channel] = (uint8_t)((source[channel] * alpha + background * (255 - alpha) + 127) / 255);
				}
			}
		}

		if (resizedPixels != nullptr)
		{
			DefaultAllocator::GetInstance().Free(resizedPixels);
		}

		MemoryStream memoryStream;
		memoryStream.Write(&thumbnailWidth, sizeof(thumbnailWidth));
		memoryStream.Write(&thumbnailHeight, sizeof(thumbnailHeight));
		{
			CompressionStream compressionStream(memoryStream);
			compressionStream.Write(rgbPixels, rgbPixelsSize);
			compressionStream.Close();
		}

		DefaultAllocator::GetInstance().Free(rgbPixels);

		encoded.Resize(memoryStream.GetSize());
		std::memcpy(encoded.Data(), memoryStream.GetBuffer(), memoryStream.GetSize());
		return true;
	}

	/// @brief
	/// @param encoded
	/// @param filterMode
	/// @return
	Texture* Thumbnail::CreateTexture(const Vector<uint8_t>& encoded, FilterMode filterMode)
	{
		if (encoded.Empty())
		{
			return nullptr;
		}

		MemoryStream memoryStream(encoded.Size());
		memoryStream.Write(encoded.Data(), encoded.Size());
		memoryStream.Seek(0, Stream::SeekOrigin::Begin);

		uint16_t width = 0;
		uint16_t height = 0;
		memoryStream.Read(&width, sizeof(width));
		memoryStream.Read(&height, sizeof(height));
		if (width == 0 || height == 0)
		{
			OUTPUT_ERROR("Thumbnail::CreateTexture: invalid size");
			return nullptr;
		}

		uint32_t pixelCount = width * height;
		uint32_t rgbPixelsSize = pixelCount * StoredComponentCount;
		uint8_t* rgbPixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(rgbPixelsSize);

		CompressionStream compressionStream(memoryStream);
		if (compressionStream.Read(rgbPixels, rgbPixelsSize) != rgbPixelsSize)
		{
			OUTPUT_ERROR("Thumbnail::CreateTexture: decompression failed");
			DefaultAllocator::GetInstance().Free(rgbPixels);
			return nullptr;
		}

		uint8_t* pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(pixelCount * TextureComponentCount);
		for (uint32_t pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
		{
			pixels[pixelIndex * TextureComponentCount + 0] = rgbPixels[pixelIndex * StoredComponentCount + 0];
			pixels[pixelIndex * TextureComponentCount + 1] = rgbPixels[pixelIndex * StoredComponentCount + 1];
			pixels[pixelIndex * TextureComponentCount + 2] = rgbPixels[pixelIndex * StoredComponentCount + 2];
			pixels[pixelIndex * TextureComponentCount + 3] = 255;
		}
		DefaultAllocator::GetInstance().Free(rgbPixels);

		Texture::CreateInfo createInfo;
		createInfo._allowReadWrite = false;

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		bool     built = texture->BuildBuffer(width, height, pixels, createInfo);
		DefaultAllocator::GetInstance().Free(pixels);

		if (built == false)
		{
			OUTPUT_ERROR("Thumbnail::CreateTexture: texture creation failed");
			DefaultAllocator::GetInstance().Delete(texture);
			return nullptr;
		}

		Sampler::CreateInfo samplerCreateInfo;
		samplerCreateInfo._filterMode = filterMode;
		texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(samplerCreateInfo));
		return texture;
	}
}
