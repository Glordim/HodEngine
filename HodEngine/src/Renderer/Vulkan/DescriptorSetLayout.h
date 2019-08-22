#ifndef __DESCRIPTOR_SET_LAYOUT_HPP__
#define __DESCRIPTOR_SET_LAYOUT_HPP__

#include <vulkan.h>

#include <string>
#include <map>

#include "spirv_cross.hpp"

class DescriptorSetLayout
{
public:

    struct Block
    {
        enum Type
        {
            Ubo,
            Texture
        };

        uint32_t binding;
        std::string name;
    };

    struct BlockUbo : Block
    {
        struct Member
        {
            std::string name;
            size_t size;
            size_t offset;
        };

        std::map<std::string, Member> memberNameToMemberMap;

        size_t size;
    };

    struct BlockTexture : Block
    {
    };

public:

    DescriptorSetLayout();
    virtual ~DescriptorSetLayout();

    VkDescriptorSetLayout GetDescriptorSetLayout() const;

    void ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource);
    void ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource);

    bool BuildDescriptorSetLayout();

    const std::vector<BlockUbo>& GetUboBlocks() const;
    const std::vector<BlockTexture>& GetTextureBlocks() const;

private:

    VkDescriptorSetLayout descriptorSetLayout;

    std::vector<BlockUbo> uboBlockVector;
    std::vector<BlockTexture> textureBlockVector;
};

#endif
