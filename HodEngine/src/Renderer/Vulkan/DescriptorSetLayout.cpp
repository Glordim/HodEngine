#include "DescriptorSetLayout.h"

#include "RendererVulkan.h"

DescriptorSetLayout::DescriptorSetLayout()
{
    this->descriptorSetLayout = VK_NULL_HANDLE;
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (this->descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(renderer->GetVkDevice(), this->descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::GetDescriptorSetLayout() const
{
    return this->descriptorSetLayout;
}

void DescriptorSetLayout::ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
{
    BlockUbo ubo;

    ubo.binding = comp.get_decoration(resource.id, spv::DecorationBinding);

    size_t uboBlockCount = this->uboBlockVector.size();
    for (size_t i = 0; i < uboBlockCount; ++i)
    {
        if (this->uboBlockVector[i].binding == ubo.binding)
            return;
    }

    ubo.name = comp.get_name(resource.id);

    const spirv_cross::SPIRType& type = comp.get_type(resource.type_id);

    ubo.rootMember.name = ubo.name;
    ubo.rootMember.offset = 0;
    ubo.rootMember.size = comp.get_declared_struct_size(type);
    if (type.array.empty() == false)
        ubo.rootMember.count = type.array[0];
    else
        ubo.rootMember.count = 1;

    this->ExtractUboSubMembers(comp, type, ubo.rootMember);

    this->uboBlockVector.push_back(std::move(ubo));
}

void DescriptorSetLayout::ExtractUboSubMembers(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& structType, BlockUbo::Member& structMember)
{
    size_t memberCount = structType.member_types.size();
    for (size_t i = 0; i < memberCount; ++i)
    {
        const spirv_cross::SPIRType& memberType = comp.get_type(structType.member_types[i]);

        BlockUbo::Member member;

        member.name = comp.get_member_name(structType.self, i);
        member.size = comp.get_declared_struct_member_size(structType, i);

        if (memberType.array.empty() == false)
            member.count = memberType.array[0];
        else
            member.count = 1;

        member.offset = comp.type_struct_member_offset(structType, i);

        if (memberType.basetype == spirv_cross::SPIRType::Struct)
        {
            this->ExtractUboSubMembers(comp, memberType, member);
        }

        structMember.childsMap.emplace(member.name, std::move(member));
    }
}

void DescriptorSetLayout::ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
{
    BlockTexture texture;

    texture.binding = comp.get_decoration(resource.id, spv::DecorationBinding);
    texture.name = comp.get_name(resource.id);

    this->textureBlockVector.push_back(std::move(texture));
}

const std::vector<DescriptorSetLayout::BlockUbo>& DescriptorSetLayout::GetUboBlocks() const
{
    return this->uboBlockVector;
}

const std::vector<DescriptorSetLayout::BlockTexture>& DescriptorSetLayout::GetTextureBlocks() const
{
    return this->textureBlockVector;
}

bool DescriptorSetLayout::BuildDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> descriptors;

    size_t uboCount = this->uboBlockVector.size();
    for (size_t i = 0; i < uboCount; ++i)
    {
        BlockUbo& ubo = this->uboBlockVector[i];

        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = ubo.binding;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        descriptors.push_back(std::move(uboLayoutBinding));
    }

    size_t textureCount = this->textureBlockVector.size();
    for (size_t i = 0; i < textureCount; ++i)
    {
        BlockTexture& texture = this->textureBlockVector[i];

        VkDescriptorSetLayoutBinding textureLayoutBinding = {};
        textureLayoutBinding.binding = texture.binding;
        textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureLayoutBinding.descriptorCount = 1;
        textureLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        textureLayoutBinding.pImmutableSamplers = nullptr;

        descriptors.push_back(std::move(textureLayoutBinding));
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = descriptors.size();
    layoutInfo.pBindings = descriptors.data();

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    if (vkCreateDescriptorSetLayout(renderer->GetVkDevice(), &layoutInfo, nullptr, &this->descriptorSetLayout) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: to create descriptor set layout!\n");
        return false;
    }

    return true;
}
