#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

namespace hod::renderer
{
	class ShaderSetDescriptorVk;
	class VkTexture;
	class BufferVk;

	/// @brief 
	class HOD_RENDERER_API DescriptorSet
	{
	public:

									DescriptorSet();
		virtual						~DescriptorSet();

		bool							SetLayout(const ShaderSetDescriptorVk* layout);
		const ShaderSetDescriptorVk*	GetLayout() const;

		VkDescriptorSet				GetDescriptorSet() const;

		void						SetUboValue(const std::string& memberName, const void* value, size_t valueSize);
		void						SetTexture(const std::string& name, const VkTexture* textureSampler);

	private:

		const ShaderSetDescriptorVk*	_descriptorSetLayout;

		VkDescriptorSet				_descriptorSet;

		std::vector<BufferVk*>		_uboBuffers;
	};
}
