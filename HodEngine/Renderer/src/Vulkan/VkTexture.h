#pragma once

#include "../Texture.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkTexture : public Texture
		{
		public:

							VkTexture();
			virtual			~VkTexture();

			virtual bool	BuildDepth(size_t width, size_t height) override;
			virtual bool	BuildBuffer(size_t width, size_t height, unsigned char* buffer) override;

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
