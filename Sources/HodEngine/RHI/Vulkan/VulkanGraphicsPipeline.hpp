#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	/// @brief
	class HOD_RHI_API VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline();
		virtual ~VulkanGraphicsPipeline();

		bool Build(const CreateInfo& createInfo) override;

		// Drawn in any render pass of the formats of the pipeline, see VulkanRhiDevice::GetCompatibleRenderPass
		VkPipeline       GetVkPipeline() const;
		VkPipelineLayout GetPipelineLayout() const;
		VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const;
		uint32_t              GetSetSpace(uint32_t set) const;
		uint32_t         GetPushConstantSize() const;

	private:
		struct NativeCreateInfo
		{
			VkPipelineShaderStageCreateInfo           _shaderStageInfos[2];
			Vector<VkVertexInputAttributeDescription> _vertexAttributeDecriptions;
			Vector<VkVertexInputBindingDescription>   _vertexBindingDescriptions;
			VkPipelineVertexInputStateCreateInfo      _vertexInput;
			VkPipelineInputAssemblyStateCreateInfo    _inputAssembly;
			VkPipelineViewportStateCreateInfo         _viewport;
			VkPipelineRasterizationStateCreateInfo    _rasterizer;
			VkPipelineMultisampleStateCreateInfo      _multisampling;
			VkPipelineDepthStencilStateCreateInfo     _depthStencil;
			VkPipelineColorBlendAttachmentState       _colorBlendAttachment;
			VkPipelineColorBlendStateCreateInfo       _colorBlend;
			VkDynamicState                            _dynamicStates[2];
			VkPipelineDynamicStateCreateInfo          _dynamic;
		};

	private:
		static bool FillNativeCreateInfo(NativeCreateInfo& nativeCreateInfo, const CreateInfo& createInfo);
		static bool BuildDescriptorSetLayout(const ShaderSetDescriptor& setDescriptor, VkDescriptorSetLayout* descriptorSetLayout);
		bool        CreatePipeline(const NativeCreateInfo& nativeCreateInfo);

	private:
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		Vector<VkDescriptorSetLayout> _descriptorSetLayouts; // indexed by space
		Vector<uint32_t>              _setSpaces;            // indexed by set
		uint32_t         _pushConstantSize = 0;

		VkPipeline _pipeline = VK_NULL_HANDLE;
	};
}
