#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API VkTexture : public Texture
	{
	public:
		VkTexture();
		~VkTexture() override;

		bool BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) override;

		Color ReadPixel(const Vector2& position) const override;
		void  CaptureReadback(CommandBuffer* commandBuffer) override;

		VkImage     GetTextureImage() const;
		VkImageView GetTextureImageView() const;
		VkSampler   GetTextureSampler() const;

	private:
		VkImage       _textureImage;
		VmaAllocation _textureImageMemory;

		VkImageView _textureImageView;
		VkSampler   _textureSampler;

		// CPU-visible copy of the texture content, kept up to date once per frame by CaptureReadback,
		// so ReadPixel() can be a plain memory read instead of an immediate, blocking GPU round-trip.
		VkBuffer      _readbackBuffer = VK_NULL_HANDLE;
		VmaAllocation _readbackBufferMemory = VK_NULL_HANDLE;
		void*         _readbackMappedData = nullptr;
		bool          _readbackReady = false;
	};
}
