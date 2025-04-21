#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include <vector>
#include <cstring>
#include <string_view>

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/ShaderGenerator/ShaderGenerator.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#undef min
#undef max
#include <spirv_cross.hpp>

namespace hod::renderer
{
	/// @brief 
	/// @param type 
	VkShader::VkShader(ShaderType type) : Shader(type)
	{
	}

	/// @brief 
	VkShader::~VkShader()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (_shaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(renderer->GetVkDevice(), _shaderModule, nullptr);
		}
	}

	bool VkShader::LoadFromSource(std::string_view source)
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		std::vector<uint8_t> shaderIR;
		if (renderer->GetShaderGenerator()->GenerateByteCode(shaderIR, GetShaderType(), source) == false)
		{
			return false;
		}

		return LoadFromIR(shaderIR.data(), (uint32_t)shaderIR.size());
	}

	/// @brief 
	/// @param data 
	/// @param size 
	/// @return 
	bool VkShader::LoadFromIR(const void* data, uint32_t size)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		{
			OUTPUT_ERROR("VkShader : Failed to create Shader Module");
			return false;
		}

		_buffer.resize(size);
		memcpy(_buffer.data(), data, size);

		if (GenerateDescriptors() == false)
		{
			vkDestroyShaderModule(renderer->GetVkDevice(), _shaderModule, nullptr);
			_shaderModule = VK_NULL_HANDLE;

			OUTPUT_ERROR("VkShader : Failed to extract descriptor");
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	VkShaderModule VkShader::GetShaderModule() const
	{
		return _shaderModule;
	}

	/// @brief 
	/// @param set 
	/// @return 
	ShaderSetDescriptorVk* VkShader::GetOrCreateSetDescriptor(uint32_t set)
	{
		ShaderSetDescriptorVk* setDescriptor = nullptr;

		auto it = _setDescriptors.find(set);
		if (it != _setDescriptors.end())
		{
			setDescriptor = (ShaderSetDescriptorVk*)it->second;
		}
		else
		{
			setDescriptor = new ShaderSetDescriptorVk();
			_setDescriptors.emplace(set, setDescriptor);
		}

		return setDescriptor;
	}

	/// @brief 
	/// @return 
	bool VkShader::GenerateDescriptors()
	{
		spirv_cross::Compiler compiler(reinterpret_cast<const uint32_t*>(_buffer.data()), _buffer.size() / sizeof(uint32_t));
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		size_t constantBufferCount = resources.push_constant_buffers.size();
		if (constantBufferCount > 0)
		{
			spirv_cross::Resource& resource = resources.push_constant_buffers[0]; // todo only first ?

			uint32_t size = 0;
			spirv_cross::SmallVector<spirv_cross::BufferRange> ranges = compiler.get_active_buffer_ranges(resource.id);
			for (const spirv_cross::BufferRange& range : ranges)
			{
				size += unsigned(range.range);
			}

			_constantDescriptor = new ShaderConstantDescriptorVk(0, size, GetShaderType());
		}

		size_t uniformBufferCount = resources.uniform_buffers.size();
		for (size_t i = 0; i < uniformBufferCount; ++i)
		{
			spirv_cross::Resource& resource = resources.uniform_buffers[i];

			uint32_t set = (uint32_t)compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			ShaderSetDescriptorVk* setDescriptor = GetOrCreateSetDescriptor(set);
			setDescriptor->ExtractBlockUbo(compiler, resource);
		}

		size_t textureCount = resources.sampled_images.size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			spirv_cross::Resource& resource = resources.sampled_images[i];

			uint32_t set = (uint32_t)compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			ShaderSetDescriptorVk* setDescriptor = GetOrCreateSetDescriptor(set);
			setDescriptor->ExtractBlockTexture(compiler, resource, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		}

		textureCount = resources.separate_samplers.size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			spirv_cross::Resource& resource = resources.separate_samplers[i];

			uint32_t set = (uint32_t)compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			ShaderSetDescriptorVk* setDescriptor = GetOrCreateSetDescriptor(set);
			setDescriptor->ExtractBlockTexture(compiler, resource, VK_DESCRIPTOR_TYPE_SAMPLER);
		}

		textureCount = resources.separate_images.size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			spirv_cross::Resource& resource = resources.separate_images[i];

			uint32_t set = (uint32_t)compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			ShaderSetDescriptorVk* setDescriptor = GetOrCreateSetDescriptor(set);
			setDescriptor->ExtractBlockTexture(compiler, resource, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		}

		return true;
	}
}
