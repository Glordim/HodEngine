#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

#include "HodEngine/Core/Document/Document.hpp"

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API ShaderSetDescriptorVk : public ShaderSetDescriptor
	{
	public:
		static VkDescriptorType TextureTypeToVkDescriptorType(BlockTexture::Type type);

	public:
		ShaderSetDescriptorVk();
		~ShaderSetDescriptorVk() override;

		VkDescriptorSetLayout GetDescriptorSetLayout() const;

		void ExtractBlockUbo(const Document::Node& parameterNode);
		void ExtractUboSubMembers(const Document::Node& fieldNode, BlockUbo::Member& structMember);

		void ExtractBlockTexture(const Document::Node& parameterNode);
		void ExtractBlockSampler(const Document::Node& parameterNode);

		bool BuildDescriptorSetLayout();

	private:
		VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
	};
}
