#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <cstring>
#include <string_view>

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

namespace hod::renderer
{
	/// @brief
	/// @param type
	VkShader::VkShader(ShaderType type)
	: Shader(type)
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

	/// @brief
	/// @param data
	/// @param Size
	/// @return
	bool VkShader::LoadFromIR(const void* data, uint32_t Size)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.codeSize = Size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		{
			OUTPUT_ERROR("VkShader : Failed to create Shader Module");
			return false;
		}

		_buffer.Resize(Size);
		memcpy(_buffer.Data(), data, Size);

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
			setDescriptor = DefaultAllocator::GetInstance().New<ShaderSetDescriptorVk>();
			_setDescriptors.emplace(set, setDescriptor);
		}

		return setDescriptor;
	}

	/// @brief
	/// @return
	bool VkShader::GenerateDescriptors()
	{
		spirv_cross::Compiler        compiler(reinterpret_cast<const uint32_t*>(_buffer.Data()), _buffer.Size() / sizeof(uint32_t));
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		size_t constantBufferCount = resources.push_constant_buffers.size();
		if (constantBufferCount > 0)
		{
			spirv_cross::Resource& resource = resources.push_constant_buffers[0]; // todo only first ?

			uint32_t                                           Size = 0;
			spirv_cross::SmallVector<spirv_cross::BufferRange> ranges = compiler.get_active_buffer_ranges(resource.id);
			for (const spirv_cross::BufferRange& range : ranges)
			{
				Size += unsigned(range.range);
			}

			_constantDescriptor = DefaultAllocator::GetInstance().New<ShaderConstantDescriptorVk>(0, Size, GetShaderType());
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
