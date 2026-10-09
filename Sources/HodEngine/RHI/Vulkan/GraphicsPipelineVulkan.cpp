#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/Vulkan/DescriptorSet.hpp"
#include "HodEngine/RHI/Vulkan/GraphicsPipelineVulkan.hpp"
#include "HodEngine/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"

#undef min
#undef max

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cassert>


#include "HodEngine/RHI/VertexInput.hpp"

namespace hod::inline rhi
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

	namespace
	{
		VkShaderStageFlags ShaderTypeToVkShaderStage(Shader::ShaderType shaderType)
		{
			switch (shaderType)
			{
				case Shader::ShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
				case Shader::ShaderType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
				default:
					// Todo
					assert(false);
					return 0;
			}
		}
	}

	/// @brief
	GraphicsPipelineVulkan::GraphicsPipelineVulkan()
	: GraphicsPipeline()
	{
		_pipelineLayout = VK_NULL_HANDLE;
	}

	/// @brief
	GraphicsPipelineVulkan::~GraphicsPipelineVulkan()
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		for (const auto& pair : _pipelines)
		{
			rhiDevice->DeferDestroy(pair.second);
		}

		if (_pipelineLayout != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_pipelineLayout);
		}

		for (VkDescriptorSetLayout descriptorSetLayout : _descriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(rhiDevice->GetVkDevice(), descriptorSetLayout, nullptr);
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
	bool GraphicsPipelineVulkan::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode,
	                       GraphicsPipeline::Topololy topololy, bool useDepth)
	{
		if (GraphicsPipelineVulkan::FillCreateInfo(_createInfo, vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			return false;
		}

		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		MergeSetDescriptors(*vertexShader, *fragmentShader);

		// _setDescriptors is ordered and contiguous: its entries are the sets 0, 1, 2...
		for (const auto& pair : _setDescriptors)
		{
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
			if (BuildDescriptorSetLayout(*pair.second, &descriptorSetLayout) == false)
			{
				return false;
			}
			_descriptorSetLayouts.PushBack(descriptorSetLayout);
		}

		Vector<VkPushConstantRange> constants;
		for (const Shader* shader : {vertexShader, fragmentShader})
		{
			const ShaderConstantDescriptor* constantDescriptor = shader->GetConstantDescriptor();
			if (constantDescriptor != nullptr)
			{
				VkPushConstantRange pushConstantRange = {};
				pushConstantRange.stageFlags = ShaderTypeToVkShaderStage(constantDescriptor->GetShaderType());
				pushConstantRange.offset = constantDescriptor->GetOffset();
				pushConstantRange.size = constantDescriptor->GetSize();
				constants.push_back(pushConstantRange);
			}
		}

		// Pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)_descriptorSetLayouts.Size();
		pipelineLayoutInfo.pSetLayouts = _descriptorSetLayouts.Data();
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)constants.Size();
		pipelineLayoutInfo.pPushConstantRanges = constants.Data();

		for (VkPushConstantRange pushConstantRange : constants)
		{
			_pushConstantSize = pushConstantRange.size; // todo
		}

		if (vkCreatePipelineLayout(rhiDevice->GetVkDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create pipeline layout!");
			return false;
		}

		return CreatePipeline(rhiDevice->GetDummyRenderPass(), true);
	}

	bool GraphicsPipelineVulkan::FillCreateInfo(CreateInfo& createInfo, const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
	                                PolygonMode polygonMode, Topololy topololy, bool useDepth)
	{
		// Shader stages
		VkPipelineShaderStageCreateInfo& vertShaderStageInfo = createInfo._shaderStageInfos[0];
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.pNext = nullptr;
		vertShaderStageInfo.flags = 0;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = ((VkShader*)vertexShader)->GetShaderModule();
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo& fragShaderStageInfo = createInfo._shaderStageInfos[1];
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.pNext = nullptr;
		fragShaderStageInfo.flags = 0;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = ((VkShader*)fragmentShader)->GetShaderModule();
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = nullptr;

		Vector<uint32_t>                           strides;
		Vector<VkVertexInputAttributeDescription>& vertexAttributeDecriptions = createInfo._vertexAttributeDecriptions;
		Vector<VkVertexInputBindingDescription>&   vertexBindingDescriptions = createInfo._vertexBindingDescriptions;

		if (vertexInputs != nullptr)
		{
			uint32_t maxBinding = 0;
			for (uint32_t i = 0; i < vertexInputCount; ++i)
			{
				maxBinding = std::max(maxBinding, vertexInputs[i]._binding);
			}

			++maxBinding;
			strides.Resize(maxBinding, 0);
			vertexBindingDescriptions.Resize(maxBinding);
			for (uint32_t i = 0; i < vertexBindingDescriptions.Size(); ++i)
			{
				VkVertexInputBindingDescription& vertexBindingDescription = vertexBindingDescriptions[i];
				vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				vertexBindingDescription.stride = 0;
			}

			vertexAttributeDecriptions.Resize(vertexInputCount);

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
			size_t inputCount = resourcesVert.stage_inputs.Size();
			vertexAttributeDecriptions.Resize(inputCount);

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
			    uint32_t Size = vertexAttributeDecriptions[i].offset;
			    vertexAttributeDecriptions[i].offset = stride;

			    stride += Size;
			}
			*/
		}

		/*
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		*/

		VkPipelineVertexInputStateCreateInfo& vertexInput = createInfo._vertexInput;
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.pNext = nullptr;
		vertexInput.flags = 0;
		vertexInput.vertexBindingDescriptionCount = (uint32_t)vertexBindingDescriptions.Size();

		vertexInput.pVertexBindingDescriptions = vertexBindingDescriptions.Data();
		vertexInput.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDecriptions.Size();
		vertexInput.pVertexAttributeDescriptions = vertexAttributeDecriptions.Data();

		// Input assembly
		VkPipelineInputAssemblyStateCreateInfo& inputAssembly = createInfo._inputAssembly;
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = nullptr;
		inputAssembly.flags = 0;
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
		VkPipelineViewportStateCreateInfo& viewport = createInfo._viewport;
		viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport.pNext = nullptr;
		viewport.flags = 0;
		viewport.viewportCount = 1;
		viewport.pViewports = nullptr;
		viewport.scissorCount = 1;
		viewport.pScissors = nullptr;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo& rasterizer = createInfo._rasterizer;
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.pNext = nullptr;
		rasterizer.flags = 0;
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
		rasterizer.cullMode = VK_CULL_MODE_NONE;                // VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_COUNTER_CLOCKWISE
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f;          // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

		// Multisampling
		VkPipelineMultisampleStateCreateInfo& multisampling = createInfo._multisampling;
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.pNext = nullptr;
		multisampling.flags = 0;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;          // Optional
		multisampling.pSampleMask = nullptr;            // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE;      // Optional

		// Depth and stencil testing
		VkPipelineDepthStencilStateCreateInfo& depthStencil = createInfo._depthStencil;
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.pNext = nullptr;
		depthStencil.flags = 0;
		depthStencil.depthTestEnable = useDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = useDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {};  // Optional

		// Color blending
		VkPipelineColorBlendAttachmentState& colorBlendAttachment = createInfo._colorBlendAttachment;
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;           // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                            // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;           // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                            // Optional

		VkPipelineColorBlendStateCreateInfo& colorBlending = createInfo._colorBlend;
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;
		colorBlending.flags = 0;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		createInfo._dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
		createInfo._dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

		VkPipelineDynamicStateCreateInfo& dynamicState = createInfo._dynamic;
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pNext = nullptr;
		dynamicState.flags = 0;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = createInfo._dynamicStates;

		return true;
	}

	VkPipeline GraphicsPipelineVulkan::CreatePipeline(VkRenderPass renderPass, bool parent)
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = _createInfo._shaderStageInfos;
		pipelineInfo.pVertexInputState = &_createInfo._vertexInput;
		pipelineInfo.pInputAssemblyState = &_createInfo._inputAssembly;
		pipelineInfo.pViewportState = &_createInfo._viewport;
		pipelineInfo.pRasterizationState = &_createInfo._rasterizer;
		pipelineInfo.pMultisampleState = &_createInfo._multisampling;
		pipelineInfo.pDepthStencilState = &_createInfo._depthStencil;
		pipelineInfo.pColorBlendState = &_createInfo._colorBlend;
		pipelineInfo.pDynamicState = &_createInfo._dynamic;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		if (parent)
		{
			pipelineInfo.flags |= VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;
		}
		else
		{
			pipelineInfo.flags |= VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = _parent;
			pipelineInfo.basePipelineIndex = -1;
		}

		VkPipeline pipeline;
		if (vkCreateGraphicsPipelines(rhiDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create graphics pipeline!");
			return VK_NULL_HANDLE;
		}
		_pipelines.PushBack({renderPass, pipeline});

		if (parent)
		{
			_parent = pipeline;
		}
		return pipeline;
	}

	/// @brief
	/// @return
	VkPipeline GraphicsPipelineVulkan::GetVkPipeline(VkRenderPass renderPass)
	{
		for (const auto& pair : _pipelines)
		{
			if (pair.first == renderPass)
			{
				return pair.second;
			}
		}
		return CreatePipeline(renderPass, false);
	}

	/// @brief
	/// @param setDescriptor
	/// @param descriptorSetLayout
	/// @return
	bool GraphicsPipelineVulkan::BuildDescriptorSetLayout(const ShaderSetDescriptor& setDescriptor, VkDescriptorSetLayout* descriptorSetLayout)
	{
		const Vector<ShaderSetDescriptor::BlockUbo>&     uboBlocks = setDescriptor.GetUboBlocks();
		const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = setDescriptor.GetTextureBlocks();

		Vector<VkDescriptorSetLayoutBinding> descriptors;

		size_t uboCount = uboBlocks.Size();
		for (size_t i = 0; i < uboCount; ++i)
		{
			const ShaderSetDescriptor::BlockUbo& ubo = uboBlocks[i];

			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = ubo._binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; // offset given when the set is bound
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.PushBack(std::move(uboLayoutBinding));
		}

		size_t textureCount = textureBlocks.Size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			const ShaderSetDescriptor::BlockTexture& texture = textureBlocks[i];

			VkDescriptorSetLayoutBinding textureLayoutBinding = {};
			textureLayoutBinding.binding = texture._binding;
			textureLayoutBinding.descriptorType = DescriptorSet::TextureTypeToVkDescriptorType(texture._type);
			textureLayoutBinding.descriptorCount = 1;
			textureLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			textureLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.PushBack(std::move(textureLayoutBinding));
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)descriptors.Size();
		layoutInfo.pBindings = descriptors.Data();

		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		if (vkCreateDescriptorSetLayout(rhiDevice->GetVkDevice(), &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: to create descriptor set layout!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param set
	/// @return
	VkDescriptorSetLayout GraphicsPipelineVulkan::GetDescriptorSetLayout(uint32_t set) const
	{
		if (set >= _descriptorSetLayouts.Size())
		{
			return VK_NULL_HANDLE;
		}
		return _descriptorSetLayouts[set];
	}

	/// @brief
	/// @return
	VkPipelineLayout GraphicsPipelineVulkan::GetPipelineLayout() const
	{
		return _pipelineLayout;
	}

	uint32_t GraphicsPipelineVulkan::GetPushConstantSize() const
	{
		return _pushConstantSize;
	}
}
