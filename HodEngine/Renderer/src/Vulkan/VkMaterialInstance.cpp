#include "VkMaterialInstance.h"
#include "VkMaterial.h"
#include "VkShader.h"

#include "RendererVulkan.h"

#include "../Mesh.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterialInstance::VkMaterialInstance() : MaterialInstance()
		{
			_material = nullptr;
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
		bool VkMaterialInstance::SetMaterial(Material* material)
		{
			if (_material == material)
			{
				return true;
			}

			_material = (VkMaterial*)material;

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			const std::map<int, DescriptorSetLayout>& descriptorSetLayoutMap = _material->GetDescriptorSetLayoutMap();

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

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterial* VkMaterialInstance::GetMaterial() const
		{
			return _material;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		std::vector<VkDescriptorSet> VkMaterialInstance::GetDescriptorSets() const
		{
			std::vector<VkDescriptorSet> descriptorSets;

			size_t descriptorSetCount = _descriptorSets.size();

			descriptorSets.reserve(descriptorSetCount);

			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				descriptorSets.push_back(_descriptorSets[i].GetDescriptorSet());
			}

			return descriptorSets;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void VkMaterialInstance::SetInt(const std::string& memberName, int value)
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
		void VkMaterialInstance::SetFloat(const std::string& memberName, float value)
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
		void VkMaterialInstance::SetVec4(const std::string& memberName, const glm::vec4& value)
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
		void VkMaterialInstance::SetMat4(const std::string& memberName, const glm::mat4& value)
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
		void VkMaterialInstance::SetTexture(const std::string& name, const Texture& value)
		{
			size_t descriptorSetCount = _descriptorSets.size();
			for (size_t i = 0; i < descriptorSetCount; ++i)
			{
				_descriptorSets[i].SetTexture(name, (VkTexture*)&value);
			}
		}
	}
}
