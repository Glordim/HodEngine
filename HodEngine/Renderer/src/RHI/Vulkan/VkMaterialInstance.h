#pragma once

#include "../MaterialInstance.h"

#include "DescriptorSet.h"
#include "BufferVk.h"

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

											VkMaterialInstance(const Material& material);
											~VkMaterialInstance() override;

			std::vector<VkDescriptorSet>	GetDescriptorSets(uint32_t setOffset, uint32_t setCount) const;

		protected:

			void							ApplyInt(const std::string& memberName, int value) override;
			void							ApplyFloat(const std::string& memberName, float value) override;
			void							ApplyVec4(const std::string& memberName, const glm::vec4& value) override;
			void							ApplyMat4(const std::string& memberName, const glm::mat4& value) override;
			void							ApplyTexture(const std::string& name, const Texture& value) override;

		private:

			std::vector<DescriptorSet>		_descriptorSets;
		};
	}
}
