#pragma once

#include <Vulkan/vulkan.h>

#include <vector>

namespace HOD
{
	namespace RENDERER
	{
		class DescriptorSetLayout;
		class VkTexture;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class DescriptorSet
		{
		public:

										DescriptorSet();
			virtual						~DescriptorSet();

			bool						SetLayout(const DescriptorSetLayout* layout);
			const DescriptorSetLayout*	GetLayout() const;

			VkDescriptorSet				GetDescriptorSet() const;

			void						SetUboValue(const std::string& memberName, const void* value, size_t valueSize);
			void						SetTexture(const std::string& name, const VkTexture* textureSampler);

		private:

			const DescriptorSetLayout*	_descriptorSetLayout;

			VkDescriptorSet				_descriptorSet;

			std::vector<VkBuffer>		_uboBuffers;
			std::vector<VkDeviceMemory>	_uboBufferMemories;
		};
	}
}
