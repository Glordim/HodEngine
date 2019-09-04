#include "VkMaterialInstance.h"
#include "VkMaterial.h"
#include "VkShader.h"

#include "RendererVulkan.h"

#include "../Mesh.h"

VkMaterialInstance::VkMaterialInstance() : MaterialInstance()
{
    this->material = nullptr;
}

VkMaterialInstance::~VkMaterialInstance()
{

}

bool VkMaterialInstance::SetMaterial(Material* material)
{
    if (this->material == material)
        return true;

    this->material = (VkMaterial*)material;

    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    const std::map<int, DescriptorSetLayout>& descriptorSetLayoutMap = this->material->GetDescriptorSetLayoutMap();

    size_t descriptorSetLayoutCount = descriptorSetLayoutMap.size();

    this->descriptorSets.resize(descriptorSetLayoutCount);

    auto it = descriptorSetLayoutMap.begin();
    auto itEnd = descriptorSetLayoutMap.end();
    int i = 0;

    while (it != itEnd)
    {
        DescriptorSet& descriptorSet = this->descriptorSets[i];

        descriptorSet.SetLayout(&it->second);

        ++i;
        ++it;
    }

    return true;
}

VkMaterial* VkMaterialInstance::GetMaterial() const
{
    return this->material;
}

std::vector<VkDescriptorSet> VkMaterialInstance::GetDescriptorSets() const
{
    std::vector<VkDescriptorSet> descriptorSets;

    size_t descriptorSetCount = this->descriptorSets.size();

    descriptorSets.reserve(descriptorSetCount);

    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        descriptorSets.push_back(this->descriptorSets[i].GetDescriptorSet());
    }

    return descriptorSets;
}

void VkMaterialInstance::SetInt(const std::string& uboName, const std::string& memberName, int value)
{
    size_t descriptorSetCount = this->descriptorSets.size();
    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        this->descriptorSets[i].SetUboValue(uboName, memberName, &value);
    }
}

void VkMaterialInstance::SetFloat(const std::string& uboName, const std::string& memberName, float value)
{
    size_t descriptorSetCount = this->descriptorSets.size();
    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        this->descriptorSets[i].SetUboValue(uboName, memberName, &value);
    }
}

void VkMaterialInstance::SetVec4(const std::string& uboName, const std::string& memberName, const glm::vec4& value)
{
    size_t descriptorSetCount = this->descriptorSets.size();
    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        this->descriptorSets[i].SetUboValue(uboName, memberName, &value);
    }
}

void VkMaterialInstance::SetMat4(const std::string& uboName, const std::string& memberName, const glm::mat4& value)
{
    size_t descriptorSetCount = this->descriptorSets.size();
    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        this->descriptorSets[i].SetUboValue(uboName, memberName, &value);
    }
}

void VkMaterialInstance::SetTexture(const std::string& name, const Texture& value)
{
    size_t descriptorSetCount = this->descriptorSets.size();
    for (size_t i = 0; i < descriptorSetCount; ++i)
    {
        this->descriptorSets[i].SetTexture(name, (VkTexture*)&value);
    }
}
