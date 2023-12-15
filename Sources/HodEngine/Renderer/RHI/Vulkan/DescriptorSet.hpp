#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

namespace hod
{
	namespace renderer
	{
		class DescriptorSetLayout;
		class VkTexture;
		class BufferVk;

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

			std::vector<BufferVk*>		_uboBuffers;
		};
	}
}
