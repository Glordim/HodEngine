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

			bool			BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
			bool			BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
			bool			BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) override;

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
