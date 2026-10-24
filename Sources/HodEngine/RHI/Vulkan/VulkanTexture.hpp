#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Texture.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API VulkanTexture : public Texture
	{
	public:
		static VkFormat ToVkFormat(PixelFormat format);

		VulkanTexture();
		~VulkanTexture() override;

		Color ReadPixel(const Vector2& position) const override;
		void  CaptureReadback(CommandBuffer* commandBuffer) override;

		VkImage     GetTextureImage() const;
		VkImageView GetTextureImageView() const;

	protected:
		bool BuildResource(const CreateInfo& createInfo, const uint8_t* pixels) override;
		bool UploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel) override;

	private:
		void Release();

	private:
		VkImage       _textureImage;
		VmaAllocation _textureImageMemory;

		VkImageView _textureImageView;

		// CPU-visible copy of the texture content, kept up to date once per frame by CaptureReadback,
		// so ReadPixel() can be a plain memory read instead of an immediate, blocking GPU round-trip.
		VkBuffer      _readbackBuffer = VK_NULL_HANDLE;
		VmaAllocation _readbackBufferMemory = VK_NULL_HANDLE;
		void*         _readbackMappedData = nullptr;
		bool          _readbackReady = false;
	};
}
