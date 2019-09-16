#include "VkTexture.h"

#include "RendererVulkan.h"

VkTexture::VkTexture() : Texture()
{
    this->textureImage = VK_NULL_HANDLE;
    this->textureImageMemory = VK_NULL_HANDLE;

    this->textureImageView = VK_NULL_HANDLE;
    this->textureSampler = VK_NULL_HANDLE;
}

VkTexture::~VkTexture()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (this->textureSampler != VK_NULL_HANDLE)
        vkDestroySampler(renderer->GetVkDevice(), this->textureSampler, nullptr);

    if (this->textureImageView != VK_NULL_HANDLE)
        vkDestroyImageView(renderer->GetVkDevice(), this->textureImageView, nullptr);

    if (this->textureImage != VK_NULL_HANDLE)
        vkDestroyImage(renderer->GetVkDevice(), this->textureImage, nullptr);
    if (this->textureImageMemory != VK_NULL_HANDLE)
        vkFreeMemory(renderer->GetVkDevice(), this->textureImageMemory, nullptr);
}

VkImage VkTexture::GetTextureImage() const
{
    return this->textureImage;
}

VkImageView VkTexture::GetTextureImageView() const
{
    return this->textureImageView;
}

VkSampler VkTexture::GetTextureSampler() const
{
    return this->textureSampler;
}

bool VkTexture::BuildDepth(size_t width, size_t height)
{
    bool ret = false;

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (renderer->CreateImage(width, height, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->textureImage, &this->textureImageMemory) == false)
        goto exit;

    if (renderer->TransitionImageLayout(this->textureImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) == false)
        goto exit;

    if (renderer->CreateImageView(this->textureImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, &this->textureImageView) == false)
        goto exit;

    ret = true;

exit:

    if (ret == false)
    {
        if (this->textureSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(renderer->GetVkDevice(), this->textureSampler, nullptr);
            this->textureSampler = VK_NULL_HANDLE;
        }

        if (this->textureImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(renderer->GetVkDevice(), this->textureImageView, nullptr);
            this->textureImageView = VK_NULL_HANDLE;
        }

        if (this->textureImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(renderer->GetVkDevice(), this->textureImage, nullptr);
            this->textureImage = VK_NULL_HANDLE;
        }

        if (this->textureImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(renderer->GetVkDevice(), this->textureImageMemory, nullptr);
            this->textureImageMemory = VK_NULL_HANDLE;
        }
    }

    return ret;
}

bool VkTexture::BuildBuffer(size_t width, size_t height, unsigned char* pixels)
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize = width * height * 4;
    void* data = nullptr;
    bool ret = false;

    if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer, &bufferMemory) == false)
        goto exit;
    
    if (vkMapMemory(renderer->GetVkDevice(), bufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Texture, unable to map memory\n");
        goto exit;
    }
    memcpy(data, pixels, static_cast<size_t>(bufferSize));
    vkUnmapMemory(renderer->GetVkDevice(), bufferMemory);

    if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->textureImage, &this->textureImageMemory) == false)
        goto exit;

    if (renderer->TransitionImageLayout(this->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) == false)
        goto exit;

    if (renderer->CopyBufferToImage(buffer, this->textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height)) == false)
        goto exit;

    if (renderer->TransitionImageLayout(this->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == false)
        goto exit;

    if (renderer->CreateImageView(this->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &this->textureImageView) == false)
        goto exit;

    if (renderer->CreateSampler(&this->textureSampler) == false)
        goto exit;

    ret = true;

exit:
    if (buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
    if (bufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);

    if (ret == false)
    {
        if (this->textureSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(renderer->GetVkDevice(), this->textureSampler, nullptr);
            this->textureSampler = VK_NULL_HANDLE;
        }

        if (this->textureImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(renderer->GetVkDevice(), this->textureImageView, nullptr);
            this->textureImageView = VK_NULL_HANDLE;
        }

        if (this->textureImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(renderer->GetVkDevice(), this->textureImage, nullptr);
            this->textureImage = VK_NULL_HANDLE;
        }

        if (this->textureImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(renderer->GetVkDevice(), this->textureImageMemory, nullptr);
            this->textureImageMemory = VK_NULL_HANDLE;
        }
    }

    return ret;
}
