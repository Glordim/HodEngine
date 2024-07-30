#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>

#include "HodEngine/Renderer/P2fT2f.hpp"

#include "HodEngine/Renderer/RHI/VertexInput.hpp"

namespace hod
{
	namespace renderer
	{
		VkFormat FormatToVkFormat[VertexInput::Format::Count] = {
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R8G8B8A8_UNORM,
		};

		uint32_t FormatToSize[VertexInput::Format::Count] = {
			2 * sizeof(float),
			4 * sizeof(float),
			1 * sizeof(uint32_t),
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterial::VkMaterial() : Material()
		{
			_graphicsPipeline = VK_NULL_HANDLE;
			_pipelineLayout = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMaterial::~VkMaterial()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			_descriptorSetLayoutMap.clear();

			if (_pipelineLayout != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(renderer->GetVkDevice(), _pipelineLayout, nullptr);
			}

			if (_graphicsPipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(renderer->GetVkDevice(), _graphicsPipeline, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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
			const std::vector<uint8_t>& byteCode = vertexShader->GetShaderBytecode();
			spirv_cross::Compiler compVert(reinterpret_cast<const uint32_t*>(byteCode.data()), byteCode.size() / sizeof(uint32_t));
			spirv_cross::ShaderResources resourcesVert = compVert.get_shader_resources();

			std::vector<VkVertexInputAttributeDescription> vertexAttributeDecriptions;

			uint32_t stride = 0;

			if (vertexInputs != nullptr)
			{
				vertexAttributeDecriptions.resize(vertexInputCount);
				for (uint32_t i = 0; i < vertexInputCount; ++i)
				{
					VkVertexInputAttributeDescription& vertexAttributeDescription = vertexAttributeDecriptions[i];
					vertexAttributeDescription.binding = 0;
					vertexAttributeDescription.location = i;
					vertexAttributeDescription.format = FormatToVkFormat[vertexInputs[i]._format];
					vertexAttributeDescription.offset = stride;

					stride += FormatToSize[vertexInputs[i]._format];
				}
			}
			else
			{
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
			}

			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			// Vertex input
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
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
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
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

			// Extract descriptorSet definition from shader bytecode
			//spirv_cross::Compiler compVert(((VkShader*)vertexShader)->GetShaderBytecode());
			//spirv_cross::ShaderResources resourcesVert = compVert.get_shader_resources();

			size_t uniformBufferCount = resourcesVert.uniform_buffers.size();
			for (size_t i = 0; i < uniformBufferCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.uniform_buffers[i];

				size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockUbo(compVert, resource);
			}

			size_t textureCount = resourcesVert.sampled_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.sampled_images[i];

				size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compVert, resource, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			}

			textureCount = resourcesVert.separate_samplers.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.separate_samplers[i];

				size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compVert, resource, VK_DESCRIPTOR_TYPE_SAMPLER);
			}

			textureCount = resourcesVert.separate_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.separate_images[i];

				size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compVert, resource, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
			}

			// Same thing for FragShader
			const std::vector<uint8_t>& byteCodeFrag = fragmentShader->GetShaderBytecode();
			spirv_cross::Compiler compFrag(reinterpret_cast<const uint32_t*>(byteCodeFrag.data()), byteCodeFrag.size() / sizeof(uint32_t));
			spirv_cross::ShaderResources resourcesFrag = compFrag.get_shader_resources();

			uniformBufferCount = resourcesFrag.uniform_buffers.size();
			for (size_t i = 0; i < uniformBufferCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.uniform_buffers[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockUbo(compFrag, resource);
			}

			textureCount = resourcesFrag.sampled_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.sampled_images[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compFrag, resource, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			}

			textureCount = resourcesFrag.separate_samplers.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.separate_samplers[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compFrag, resource, VK_DESCRIPTOR_TYPE_SAMPLER);
			}

			textureCount = resourcesFrag.separate_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.separate_images[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				//if (set <= 1)
				//	continue;

				DescriptorSetLayout& descriptorSetLayout = _descriptorSetLayoutMap[(uint32_t)set];

				descriptorSetLayout.ExtractBlockTexture(compFrag, resource, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
			}


			// And build each DescriptorSetLayout
			std::vector<VkDescriptorSetLayout> layouts;

			auto it = _descriptorSetLayoutMap.begin();
			auto itEnd = _descriptorSetLayoutMap.end();

			while (it != itEnd)
			{
				it->second.BuildDescriptorSetLayout();

				layouts.push_back(it->second.GetDescriptorSetLayout());

				++it;
			}

			std::vector<VkPushConstantRange> constants;

			size_t constantBufferCount = resourcesVert.push_constant_buffers.size();
			for (size_t i = 0; i < constantBufferCount; ++i)
			{
				uint32_t size = 0;

				spirv_cross::Resource& resource = resourcesVert.push_constant_buffers[i];

				spirv_cross::SmallVector<spirv_cross::BufferRange> ranges = compVert.get_active_buffer_ranges(resource.id);
				for (const spirv_cross::BufferRange& range : ranges)
				{
					size += unsigned(range.range);
				}

				VkPushConstantRange vkPushConstantRange;
				vkPushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
				vkPushConstantRange.offset = 0;
				vkPushConstantRange.size = size;

				constants.push_back(vkPushConstantRange);
			}

			constantBufferCount = resourcesFrag.push_constant_buffers.size();
			for (size_t i = 0; i < constantBufferCount; ++i)
			{
				uint32_t size = 0;

				spirv_cross::Resource& resource = resourcesFrag.push_constant_buffers[i];

				spirv_cross::SmallVector<spirv_cross::BufferRange> ranges = compFrag.get_active_buffer_ranges(resource.id);
				for (const spirv_cross::BufferRange& range : ranges)
				{
					size += unsigned(range.range);
				}

				VkPushConstantRange vkPushConstantRange;
				vkPushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
				vkPushConstantRange.offset = 0;
				vkPushConstantRange.size = size;

				constants.push_back(vkPushConstantRange);
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkPipeline VkMaterial::GetGraphicsPipeline() const
		{
			return _graphicsPipeline;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkPipelineLayout VkMaterial::GetPipelineLayout() const
		{
			return _pipelineLayout;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::map<int, DescriptorSetLayout>& VkMaterial::GetDescriptorSetLayoutMap() const
		{
			return _descriptorSetLayoutMap;
		}
	}
}
