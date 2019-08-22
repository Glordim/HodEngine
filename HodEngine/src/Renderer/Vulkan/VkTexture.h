#ifndef __VK_TEXTURE_HPP__
#define __VK_TEXTURE_HPP__

#include "../Texture.h"

#include <vulkan.h>

class VkTexture : public Texture
{
public:
    VkTexture();
    virtual ~VkTexture();
    
    virtual bool BuildBuffer(size_t width, size_t height, unsigned char* buffer) override;

    VkImage GetTextureImage() const;
    VkImageView GetTextureImageView() const;
    VkSampler GetTextureSampler() const;

private:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;
    VkSampler textureSampler;
};

#endif
