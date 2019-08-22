#include "VkMaterial.h"
#include "VkShader.h"

#include "RendererVulkan.h"

#include "../Mesh.h"

#include "spirv_cross.hpp"

VkMaterial::VkMaterial() : Material()
{
    this->graphicsPipeline = VK_NULL_HANDLE;
    this->pipelineLayout = VK_NULL_HANDLE;
}

VkMaterial::~VkMaterial()
{
    RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

    size_t descriptorSetLayoutCount = this->descriptorSetLayouts.size();
    for (int i = 0; i < descriptorSetLayoutCount; ++i)
    {
        /*
        VkDescriptorSetLayout descriptorSetLayout = this->descriptorSetLayouts[i];

        if (descriptorSetLayout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(renderer->GetVkDevice(), descriptorSetLayout, nullptr);
        */
    }
    this->descriptorSetLayouts.clear();
    this->descriptorSetLayouts.shrink_to_fit();

    if (this->pipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(renderer->GetVkDevice(), this->pipelineLayout, nullptr);

    if (this->graphicsPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(renderer->GetVkDevice(), this->graphicsPipeline, nullptr);
}

bool VkMaterial::Build(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy)
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

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex_3P_3C_3N_2UV_3TA);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription posAttribute;
    posAttribute.binding = 0;
    posAttribute.location = 0;
    posAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    posAttribute.offset = offsetof(Vertex_3P_3C_3N_2UV_3TA, pos);

    VkVertexInputAttributeDescription colorAttribute;
    colorAttribute.binding = 0;
    colorAttribute.location = 1;
    colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttribute.offset = offsetof(Vertex_3P_3C_3N_2UV_3TA, color);

    VkVertexInputAttributeDescription normalAttribute;
    normalAttribute.binding = 0;
    normalAttribute.location = 2;
    normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttribute.offset = offsetof(Vertex_3P_3C_3N_2UV_3TA, normal);

    VkVertexInputAttributeDescription uvAttribute;
    uvAttribute.binding = 0;
    uvAttribute.location = 3;
    uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
    uvAttribute.offset = offsetof(Vertex_3P_3C_3N_2UV_3TA, uv);

    VkVertexInputAttributeDescription tangentAttribute;
    tangentAttribute.binding = 0;
    tangentAttribute.location = 4;
    tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    tangentAttribute.offset = offsetof(Vertex_3P_3C_3N_2UV_3TA, tangent);

    VkVertexInputAttributeDescription vertexAttributes[] = { posAttribute, colorAttribute, normalAttribute, uvAttribute, tangentAttribute };

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 5;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes;

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    if (topololy == Topololy::TRIANGLE)
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    else if (topololy == Topololy::LINE)
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (topololy == Topololy::POINT)
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    else
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewports and scissors
    VkExtent2D swapChainExtent = renderer->GetSwapChainExtent();

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //VK_FRONT_FACE_COUNTER_CLOCKWISE
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

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
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
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Extract descriptorSet definition from shader bytecode
    spirv_cross::Compiler compVert(((VkShader*)vertexShader)->GetShaderBytecode());
    spirv_cross::ShaderResources resourcesVert = compVert.get_shader_resources();

    size_t uniformBufferCount = resourcesVert.uniform_buffers.size();
    for (size_t i = 0; i < uniformBufferCount; ++i)
    {
        spirv_cross::Resource& resource = resourcesVert.uniform_buffers[i];

        size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

        if (this->descriptorSetLayouts.size() <= set)
        {
            this->descriptorSetLayouts.resize(set + 1);
        }

        DescriptorSetLayout& descriptorSetLayout = this->descriptorSetLayouts[set];

        descriptorSetLayout.ExtractBlockUbo(compVert, resource);
    }

    size_t textureCount = resourcesVert.separate_samplers.size();
    for (size_t i = 0; i < textureCount; ++i)
    {
        spirv_cross::Resource& resource = resourcesVert.separate_samplers[i];

        size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

        if (this->descriptorSetLayouts.size() <= set)
        {
            this->descriptorSetLayouts.resize(set + 1);
        }

        DescriptorSetLayout& descriptorSetLayout = this->descriptorSetLayouts[set];

        descriptorSetLayout.ExtractBlockTexture(compVert, resource);
    }

    // And build each DescriptorSetLayout
    std::vector<VkDescriptorSetLayout> layouts;

    size_t descriptorSetLayoutCount = this->descriptorSetLayouts.size();
    for (size_t i = 0; i < descriptorSetLayoutCount; ++i)
    {
        this->descriptorSetLayouts[i].BuildDescriptorSetLayout();

        layouts.push_back(this->descriptorSetLayouts[i].GetDescriptorSetLayout());
    }

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layouts.size();
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(renderer->GetVkDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create pipeline layout!");
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
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;//&dynamicState; // nullptr; // Optional
    pipelineInfo.layout = this->pipelineLayout;
    pipelineInfo.renderPass = renderer->GetRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(renderer->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->graphicsPipeline) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create graphics pipeline!\n");
        return false;
    }

    return true;
}

VkPipeline VkMaterial::GetGraphicsPipeline() const
{
    return this->graphicsPipeline;
}

VkPipelineLayout VkMaterial::GetPipelineLayout() const
{
    return this->pipelineLayout;
}

const std::vector<DescriptorSetLayout>& VkMaterial::GetDescriptorSetLayouts() const
{
    return this->descriptorSetLayouts;
}
