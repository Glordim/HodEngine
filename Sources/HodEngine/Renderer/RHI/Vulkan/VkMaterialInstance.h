#pragma once

#include "HodEngine/Renderer/RHI/MaterialInstance.h"

#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSet.h"
#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.h"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
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
			void							ApplyVec4(const std::string& memberName, const Vector4& value) override;
			void							ApplyMat4(const std::string& memberName, const Matrix4& value) override;
			void							ApplyTexture(const std::string& name, const Texture& value) override;

		private:

			std::vector<DescriptorSet>		_descriptorSets;
		};
	}
}
