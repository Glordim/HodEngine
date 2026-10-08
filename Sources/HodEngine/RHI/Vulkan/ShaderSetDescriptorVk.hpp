#pragma once
#include "HodEngine/RHI/Export.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/Core/Document/Document.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API ShaderSetDescriptorVk : public ShaderSetDescriptor
	{
	public:
		static VkDescriptorType TextureTypeToVkDescriptorType(BlockTexture::Type type);

	public:
		ShaderSetDescriptorVk();
		~ShaderSetDescriptorVk() override;

		VkDescriptorSetLayout GetDescriptorSetLayout() const;

		void ExtractBlockUbo(const DocumentNode& parameterNode);
		void ExtractUboSubMembers(const DocumentNode& fieldNode, BlockUbo::Member& structMember);

		void ExtractBlockTexture(const DocumentNode& parameterNode);
		void ExtractBlockSampler(const DocumentNode& parameterNode);

		bool BuildDescriptorSetLayout();

	private:
		VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
	};
}
