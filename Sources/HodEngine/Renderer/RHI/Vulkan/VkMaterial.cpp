#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>

#include "HodEngine/Renderer/P2fT2f.hpp"

#include "HodEngine/Renderer/RHI/VertexInput.hpp"

namespace hod::renderer
{
	VkFormat FormatToVkFormat[VertexInput::Format::Count] = {
		VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R8G8B8A8_UNORM,
	};

	uint32_t FormatToSize[VertexInput::Format::Count] = {
		1 * sizeof(float),
		2 * sizeof(float),
		4 * sizeof(float),
		1 * sizeof(uint32_t),
	};

	/// @brief 
	VkMaterial::VkMaterial() : Material()
	{
		_graphicsPipeline = VK_NULL_HANDLE;
		_pipelineLayout = VK_NULL_HANDLE;
	}

	/// @brief 
	VkMaterial::~VkMaterial()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (_pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(renderer->GetVkDevice(), _pipelineLayout, nullptr);
		}

		if (_graphicsPipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(renderer->GetVkDevice(), _graphicsPipeline, nullptr);
		}
	}

	/// @brief 
	/// @param vertexInputs 
	/// @param vertexInputCount 
	/// @param vertexShader 
	/// @param fragmentShader 
	/// @param polygonMode 
	/// @param topololy 
	/// @param useDepth 
	/// @return 
	bool VkMaterial::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = ((VkShader*)vertexShader)->GetShaderModule();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = ((VkShader*)fragmentShader)->GetShaderModule();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Extract descriptorSet definition from shader bytecode
		const Vector<uint8_t>& byteCode = vertexShader->GetShaderBytecode();
		spirv_cross::Compiler compVert(reinterpret_cast<const uint32_t*>(byteCode.data()), byteCode.size() / sizeof(uint32_t));
		spirv_cross::ShaderResources resourcesVert = compVert.get_shader_resources();

		Vector<uint32_t> strides;
		Vector<VkVertexInputAttributeDescription> vertexAttributeDecriptions;
		Vector<VkVertexInputBindingDescription> vertexBindingDescriptions;

		if (vertexInputs != nullptr)
		{
			uint32_t maxBinding = 0;
			for (uint32_t i = 0; i < vertexInputCount; ++i)
			{
				maxBinding = std::max(maxBinding, vertexInputs[i]._binding);
			}

			++maxBinding;
			strides.resize(maxBinding, 0);
			vertexBindingDescriptions.resize(maxBinding);
			for (uint32_t i = 0; i < vertexBindingDescriptions.size(); ++i)
			{
				VkVertexInputBindingDescription& vertexBindingDescription = vertexBindingDescriptions[i];
				vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				vertexBindingDescription.stride = 0;
			}

			vertexAttributeDecriptions.resize(vertexInputCount);

			for (uint32_t i = 0; i < vertexInputCount; ++i)
			{
				uint32_t binding = vertexInputs[i]._binding;

				VkVertexInputAttributeDescription& vertexAttributeDescription = vertexAttributeDecriptions[i];
				vertexAttributeDescription.binding = binding;
				vertexAttributeDescription.location = i;
				vertexAttributeDescription.format = FormatToVkFormat[vertexInputs[i]._format];
				vertexAttributeDescription.offset = strides[binding];

				VkVertexInputBindingDescription& vertexBindingDescription = vertexBindingDescriptions[binding];
				vertexBindingDescription.binding = binding;
				vertexBindingDescription.stride += FormatToSize[vertexInputs[i]._format];

				strides[binding] += FormatToSize[vertexInputs[i]._format];
			}
		}
		else
		{
			/*
			size_t inputCount = resourcesVert.stage_inputs.size();
			vertexAttributeDecriptions.resize(inputCount);

			for (size_t i = 0; i < inputCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.stage_inputs[i];
				const spirv_cross::SPIRType& type = compVert.get_type_from_variable(resource.id);

				uint32_t location = compVert.get_decoration(resource.id, spv::DecorationLocation);

				VkVertexInputAttributeDescription& vertexAttributeDescription = vertexAttributeDecriptions[location];
				vertexAttributeDescription.binding = 0;
				vertexAttributeDescription.location = location;
				if (type.vecsize == 1)
				{
					vertexAttributeDescription.format = VK_FORMAT_R32_SFLOAT;
				}
				else if (type.vecsize == 2)
				{
					vertexAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
				}
				else if (type.vecsize == 3)
				{
					vertexAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				}
				else if (type.vecsize == 4)
				{
					vertexAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				}
				else
				{
					vertexAttributeDescription.format = VK_FORMAT_UNDEFINED;
				}

				vertexAttributeDescription.offset = (type.width / 8) * type.vecsize;
			}

			for (size_t i = 0; i < inputCount; ++i)
			{
				uint32_t size = vertexAttributeDecriptions[i].offset;
				vertexAttributeDecriptions[i].offset = stride;

				stride += size;
			}
			*/
		}

		/*
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		*/

		// Vertex input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexBindingDescriptions.size();;
		vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDecriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDecriptions.data();

		// Input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		if (topololy == Topololy::TRIANGLE)
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
		else if (topololy == Topololy::TRIANGLE_FAN)
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		}
		else if (topololy == Topololy::LINE)
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		}
		else if (topololy == Topololy::LINE_STRIP)
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		}
		else if (topololy == Topololy::POINT)
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		}
		else
		{
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewports and scissors
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = nullptr;
		viewportState.scissorCount = 1;
		viewportState.pScissors = nullptr;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		if (polygonMode == PolygonMode::Fill)
		{
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		}
		else if (polygonMode == PolygonMode::Line)
		{
			rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
		}
		else
		{
			rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
		}
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //VK_FRONT_FACE_COUNTER_CLOCKWISE
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Depth and stencil testing
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		if (useDepth == true && false)
		{
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f; // Optional
			depthStencil.maxDepthBounds = 1.0f; // Optional
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {}; // Optional
			depthStencil.back = {}; // Optional
		}

		// Color blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		for (const auto& pair : vertexShader->GetSetDescriptors())
		{
			_setDescriptors[pair.first] = pair.second;
		}
		for (const auto& pair : fragmentShader->GetSetDescriptors())
		{
			_setDescriptors[pair.first] = pair.second;
		}

		Vector<VkDescriptorSetLayout> layouts;
		for (const auto& pair : _setDescriptors)
		{
			static_cast<ShaderSetDescriptorVk*>(pair.second)->BuildDescriptorSetLayout();
			layouts.push_back(static_cast<const ShaderSetDescriptorVk*>(pair.second)->GetDescriptorSetLayout());
		}

		Vector<VkPushConstantRange> constants;
		if (vertexShader->GetConstantDescriptor() != nullptr)
		{
			constants.push_back(static_cast<const ShaderConstantDescriptorVk*>(vertexShader->GetConstantDescriptor())->GetPushConstantRange());
		}
		if (fragmentShader->GetConstantDescriptor() != nullptr)
		{
			constants.push_back(static_cast<const ShaderConstantDescriptorVk*>(fragmentShader->GetConstantDescriptor())->GetPushConstantRange());
		}

		// Pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)layouts.size();
		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)constants.size();
		pipelineLayoutInfo.pPushConstantRanges = constants.data();

		if (vkCreatePipelineLayout(renderer->GetVkDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create pipeline layout!");
			return false;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = renderer->GetContext()->GetRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(renderer->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create graphics pipeline!");
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	VkPipeline VkMaterial::GetGraphicsPipeline() const
	{
		return _graphicsPipeline;
	}

	/// @brief 
	/// @return 
	VkPipelineLayout VkMaterial::GetPipelineLayout() const
	{
		return _pipelineLayout;
	}
}
