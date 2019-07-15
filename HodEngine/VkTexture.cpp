#include "VkTexture.h"

#include "RendererVulkan.h"

VkTexture::VkTexture() : Texture()
{
    this->textureImage = VK_NULL_HANDLE;
    this->textureImageMemory = VK_NULL_HANDLE;
}

VkTexture::~VkTexture()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (this->textureImage != VK_NULL_HANDLE)
        vkDestroyImage(renderer->GetVkDevice(), this->textureImage, nullptr);
    if (this->textureImageMemory != VK_NULL_HANDLE)
        vkFreeMemory(renderer->GetVkDevice(), this->textureImageMemory, nullptr);
}

VkImage VkTexture::GetTextureImage() const
{
    return this->textureImage;
}

bool VkTexture::BuildBuffer(size_t width, size_t height, unsigned char* pixels)
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize = width * height * 4;

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer, &bufferMemory) == false)
        return false;

    void* data = nullptr;
    vkMapMemory(renderer->GetVkDevice(), bufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(bufferSize));
    vkUnmapMemory(renderer->GetVkDevice(), bufferMemory);

    if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->textureImage, &this->textureImageMemory) == false)
        return false;

    // TODO Copy data from Buffer to Image

    if (buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
    if (bufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);

    return true;
}
