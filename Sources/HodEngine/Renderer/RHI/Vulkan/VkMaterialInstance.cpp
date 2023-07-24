#include "HodEngine/Renderer/RHI/Vulkan/VkMaterialInstance.h"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.h"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.h"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterialInstance::VkMaterialInstance(const Material& material)
			: MaterialInstance(material)
		{
			const VkMaterial* vkMaterial = static_cast<const VkMaterial*>(&material);
			const std::map<int, DescriptorSetLayout>& descriptorSetLayoutMap = vkMaterial->GetDescriptorSetLayoutMap();

			size_t descriptorSetLayoutCount = descriptorSetLayoutMap.size();

			_descriptorSets.resize(descriptorSetLayoutCount);

			auto it = descriptorSetLayoutMap.begin();
			auto itEnd = descriptorSetLayoutMap.end();
			int i = 0;

			while (it != itEnd)
			{
				DescriptorSet& descriptorSet = _descriptorSets[i];

				descriptorSet.SetLayout(&it->second);

				++i;
				++it;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterialInstance::~VkMaterialInstance()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		std::vector<VkDescriptorSet> VkMaterialInstance::GetDescriptorSets(uint32_t setOffset, uint32_t setCount) const
		{
			if (setCount > _descriptorSets.size() - setOffset)
			{
				setCount = _descriptorSets.size() - setOffset;
			}

			std::vector<VkDescriptorSet> descriptorSets(setCount);

			for (size_t i = 0; i < setCount; ++i)
			{
				descriptorSets[i] = _descriptorSets[setOffset + i].GetDescriptorSet();
			}

			return descriptorSets;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::ApplyInt(const std::string& memberName, int value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::ApplyFloat(const std::string& memberName, float value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::ApplyVec4(const std::string& memberName, const Vector4& value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::ApplyMat4(const std::string& memberName, const Matrix4& value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::ApplyTexture(const std::string& name, const Texture& value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetTexture(name, (VkTexture*)&value);
			}
		}
	}
}
