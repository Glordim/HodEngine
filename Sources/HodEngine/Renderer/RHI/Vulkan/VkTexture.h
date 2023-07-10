#pragma once

#include "HodEngine/Renderer/RHI/Texture.h"

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
			virtual			~VkTexture();

			virtual bool	BuildColor(size_t width, size_t height) override;
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
