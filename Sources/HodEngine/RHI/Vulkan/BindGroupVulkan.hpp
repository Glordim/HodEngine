#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/BindGroup.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	class Buffer;
	class GraphicsPipelineVulkan;
	class Texture;

	/// @brief
	class HOD_RHI_API BindGroupVulkan : public BindGroup
	{
	public:
		BindGroupVulkan() = default;
		~BindGroupVulkan() override;

		bool Build(const GraphicsPipelineVulkan& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount, const Texture* const* textures,
		           uint32_t textureCount);

		VkDescriptorSet GetDescriptorSet() const;

		// Vulkan takes the dynamic offsets ordered by binding number, callers give them in the order of the set's uniform blocks:
		// entry i is the index of the uniform block whose offset comes in position i
		const Vector<uint32_t>& GetUniformBufferOrder() const;

	private:
		VkDescriptorSet  _descriptorSet = VK_NULL_HANDLE;
		Vector<uint32_t> _uniformBufferOrder;
	};
}
