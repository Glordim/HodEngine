#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/String.hpp"

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

		void						SetUboValue(const String& memberName, const void* value, size_t valueSize);
		void						SetTexture(const String& name, const VkTexture* textureSampler);

	private:

		const ShaderSetDescriptorVk*	_descriptorSetLayout;

		VkDescriptorSet				_descriptorSet;

		Vector<BufferVk*>		_uboBuffers;
	};
}
