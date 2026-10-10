#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/BindGroup.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	class Buffer;
	class VulkanGraphicsPipeline;
	class Texture;

	/// @brief
	class HOD_RHI_API VulkanBindGroup : public BindGroup
	{
	public:
		static VkDescriptorType TextureTypeToVkDescriptorType(ShaderSetDescriptor::BlockTexture::Type type);

	public:
		VulkanBindGroup() = default;
		~VulkanBindGroup() override;

		bool Build(const VulkanGraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
		           const TextureBinding* textureBindings, uint32_t textureBindingCount);

		VkDescriptorSet GetDescriptorSet() const;

		// Vulkan takes the dynamic offsets ordered by binding number, callers give them in the order of the set's uniform blocks:
		// entry i is the index of the uniform block whose offset comes in position i
		const Vector<uint32_t>& GetUniformBufferOrder() const;

	private:
		VkDescriptorSet  _descriptorSet = VK_NULL_HANDLE;
		Vector<uint32_t> _uniformBufferOrder;
	};
}
