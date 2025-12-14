#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Material.hpp"

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API VkMaterial : public Material
	{
	public:
		VkMaterial();
		virtual ~VkMaterial();

		virtual bool Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill,
		                   Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

		VkPipeline       GetGraphicsPipeline(VkRenderPass renderPass);
		VkPipelineLayout GetPipelineLayout() const;
		uint32_t         GetPushConstantSize() const;

	private:
		struct CreateInfo
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
		static bool FillCreateInfo(CreateInfo& createInfo, const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
		                           PolygonMode polygonMode, Topololy topololy, bool useDepth);
		VkPipeline  CreatePipeline(VkRenderPass renderPass, bool parent);

	private:
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		uint32_t         _pushConstantSize = 0;

		CreateInfo _createInfo; // Store all create info to be able to create new Pipeline with different renderpass (may be removed if use DynamicRendering VL 1.3)

		VkPipeline                                  _parent = VK_NULL_HANDLE;
		Vector<std::pair<VkRenderPass, VkPipeline>> _pipelines;
	};

	constexpr size_t toto = sizeof(VkMaterial);
}
