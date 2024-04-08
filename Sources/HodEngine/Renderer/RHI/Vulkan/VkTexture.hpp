#pragma once

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkTexture : public Texture
		{
		public:

							VkTexture();
							~VkTexture() override;

			bool			BuildColor(size_t width, size_t height, bool allowReadWrite = false) override;
			bool			BuildDepth(size_t width, size_t height) override;
			bool			BuildBuffer(size_t width, size_t height, unsigned char* buffer) override;

			Color			ReadPixel(const Vector2& position) const override;

			VkImage			GetTextureImage() const;
			VkImageView		GetTextureImageView() const;
			VkSampler		GetTextureSampler() const;

		private:

			VkImage			_textureImage;
			VkDeviceMemory	_textureImageMemory;

			VkImageView		_textureImageView;
			VkSampler		_textureSampler;
		};
	}
}
