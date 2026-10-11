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

		VkPipeline       GetVkPipeline(VkRenderPass renderPass);
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
			VkGraphicsPipelineCreateInfo              _pipelineInfo;
		};

	private:
		static bool FillNativeCreateInfo(NativeCreateInfo& nativeCreateInfo, const CreateInfo& createInfo);
		static bool BuildDescriptorSetLayout(const ShaderSetDescriptor& setDescriptor, VkDescriptorSetLayout* descriptorSetLayout);
		VkPipeline  CreatePipeline(VkRenderPass renderPass, bool parent);

	private:
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		Vector<VkDescriptorSetLayout> _descriptorSetLayouts; // indexed by space
		Vector<uint32_t>              _setSpaces;            // indexed by set
		uint32_t         _pushConstantSize = 0;

		NativeCreateInfo _nativeCreateInfo; // Store all create info to be able to create new Pipeline with different renderpass (may be removed if use DynamicRendering VK 1.3)

		VkPipeline                                  _parent = VK_NULL_HANDLE;
		Vector<std::pair<VkRenderPass, VkPipeline>> _pipelines;
	};

	constexpr size_t toto = sizeof(VulkanGraphicsPipeline);
}
