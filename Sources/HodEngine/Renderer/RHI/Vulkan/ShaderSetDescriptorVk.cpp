#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

namespace hod::renderer
{
	/// @brief 
	/// @param type 
	/// @return 
	VkDescriptorType ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(BlockTexture::Type type)
	{
		switch (type)
		{
		case BlockTexture::Type::Sampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;

		case BlockTexture::Type::Texture:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		case BlockTexture::Type::Combined:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		
		default:
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	/// @brief 
	ShaderSetDescriptorVk::ShaderSetDescriptorVk()
	{
		_descriptorSetLayout = VK_NULL_HANDLE;
	}

	/// @brief 
	ShaderSetDescriptorVk::~ShaderSetDescriptorVk()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (_descriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(renderer->GetVkDevice(), _descriptorSetLayout, nullptr);
		}
	}

	/// @brief 
	/// @return 
	VkDescriptorSetLayout ShaderSetDescriptorVk::GetDescriptorSetLayout() const
	{
		return _descriptorSetLayout;
	}

	/// @brief 
	/// @param comp 
	/// @param resource 
	void ShaderSetDescriptorVk::ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
	{
		BlockUbo ubo;
		ubo._binding = comp.get_decoration(resource.id, spv::DecorationBinding);

		size_t uboBlockCount = _uboBlockVector.size();
		for (size_t i = 0; i < uboBlockCount; ++i)
		{
			if (_uboBlockVector[i]._binding == ubo._binding)
				return;
		}

		ubo._name = comp.get_name(resource.id);

		const spirv_cross::SPIRType& type = comp.get_type(resource.type_id);

		ubo._rootMember._name = ubo._name;
		ubo._rootMember._offset = 0;
		ubo._rootMember._size = comp.get_declared_struct_size(type);
		if (type.array.empty() == false)
		{
			ubo._rootMember._count = type.array[0];
		}
		else
		{
			ubo._rootMember._count = 1;
		}

		ExtractUboSubMembers(comp, type, ubo._rootMember);

		_uboBlockVector.push_back(std::move(ubo));
	}

	/// @brief 
	/// @param comp 
	/// @param structType 
	/// @param structMember 
	void ShaderSetDescriptorVk::ExtractUboSubMembers(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& structType, BlockUbo::Member& structMember)
	{
		size_t memberCount = structType.member_types.size();
		for (size_t i = 0; i < memberCount; ++i)
		{
			const spirv_cross::SPIRType& memberType = comp.get_type(structType.member_types[i]);

			BlockUbo::Member member;

			member._name = comp.get_member_name(structType.self, (uint32_t)i);
			member._size = comp.get_declared_struct_member_size(structType, (uint32_t)i);

			if (memberType.array.empty() == false)
				member._count = memberType.array[0];
			else
				member._count = 1;

			member._offset = comp.type_struct_member_offset(structType, (uint32_t)i);

			if (memberType.basetype == spirv_cross::SPIRType::Struct)
			{
				ExtractUboSubMembers(comp, memberType, member);
			}
			else if (memberType.basetype == spirv_cross::SPIRType::Float)
			{
				if (memberType.vecsize == 1)
				{
					member._memberType = BlockUbo::MemberType::Float;
				}
				else if (memberType.vecsize == 2)
				{
					member._memberType = BlockUbo::MemberType::Float2;
				}
				else if (memberType.vecsize == 4)
				{
					member._memberType = BlockUbo::MemberType::Float4;
				}
			}

			structMember._childsMap.emplace(member._name, std::move(member));
		}
	}

	/// @brief 
	/// @param comp 
	/// @param resource 
	/// @param type 
	void ShaderSetDescriptorVk::ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource, VkDescriptorType type)
	{
		BlockTexture texture;

		texture._binding = comp.get_decoration(resource.id, spv::DecorationBinding);
		texture._name = comp.get_name(resource.id);

		switch (type)
		{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
			texture._type = BlockTexture::Type::Sampler;
			break;

		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			texture._type = BlockTexture::Type::Texture;
			break;

		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			texture._type = BlockTexture::Type::Combined;
			break;
		
		default:
			return;
		}
		
		_textureBlockVector.push_back(std::move(texture));
	}

	/// @brief 
	/// @return 
	bool ShaderSetDescriptorVk::BuildDescriptorSetLayout()
	{
		Vector<VkDescriptorSetLayoutBinding> descriptors;

		size_t uboCount = _uboBlockVector.size();
		for (size_t i = 0; i < uboCount; ++i)
		{
			BlockUbo& ubo = _uboBlockVector[i];

			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = ubo._binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.push_back(std::move(uboLayoutBinding));
		}

		size_t textureCount = _textureBlockVector.size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			BlockTexture& texture = _textureBlockVector[i];

			VkDescriptorSetLayoutBinding textureLayoutBinding = {};
			textureLayoutBinding.binding = texture._binding;
			textureLayoutBinding.descriptorType = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
			textureLayoutBinding.descriptorCount = 1;
			textureLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			textureLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.push_back(std::move(textureLayoutBinding));
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)descriptors.size();
		layoutInfo.pBindings = descriptors.data();

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateDescriptorSetLayout(renderer->GetVkDevice(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: to create descriptor set layout!");
			return false;
		}

		return true;
	}
}
