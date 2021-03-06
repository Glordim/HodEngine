#pragma once

#include "../MaterialInstance.h"

#include "DescriptorSet.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
	{
		class VkMaterial;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkMaterialInstance : public MaterialInstance
		{
		public:

											VkMaterialInstance();
			virtual							~VkMaterialInstance();

			virtual bool					SetMaterial(Material* material) override;

			virtual void					SetInt(const std::string& memberName, int value) override;
			virtual void					SetFloat(const std::string& memberName, float value) override;
			virtual void					SetVec4(const std::string& memberName, const glm::vec4& value) override;
			virtual void					SetMat4(const std::string& memberName, const glm::mat4& value) override;
			virtual void					SetTexture(const std::string& name, const Texture& value) override;

			VkMaterial*						GetMaterial() const;
			std::vector<VkDescriptorSet>	GetDescriptorSets() const;

		private:

			VkMaterial*						_material;

			VkBuffer						_uniformBuffer;
			VkDeviceMemory					_uniformBufferMemory;

			std::vector<DescriptorSet>		_descriptorSets;
		};
	}
}
