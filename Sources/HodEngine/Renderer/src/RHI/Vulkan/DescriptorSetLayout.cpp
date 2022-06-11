#include "DescriptorSetLayout.h"

#include "RendererVulkan.h"

#include <HodEngine/Core/Src/Output.h>

#include <SPIRV-Cross/spirv_cross.hpp>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DescriptorSetLayout::DescriptorSetLayout()
		{
			_descriptorSetLayout = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DescriptorSetLayout::~DescriptorSetLayout()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (_descriptorSetLayout != VK_NULL_HANDLE)
				vkDestroyDescriptorSetLayout(renderer->GetVkDevice(), _descriptorSetLayout, nullptr);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkDescriptorSetLayout DescriptorSetLayout::GetDescriptorSetLayout() const
		{
			return _descriptorSetLayout;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DescriptorSetLayout::ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DescriptorSetLayout::ExtractUboSubMembers(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& structType, BlockUbo::Member& structMember)
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

				structMember._childsMap.emplace(member._name, std::move(member));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DescriptorSetLayout::ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
		{
			BlockTexture texture;

			texture._binding = comp.get_decoration(resource.id, spv::DecorationBinding);
			texture._name = comp.get_name(resource.id);

			_textureBlockVector.push_back(std::move(texture));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<DescriptorSetLayout::BlockUbo>& DescriptorSetLayout::GetUboBlocks() const
		{
			return _uboBlockVector;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<DescriptorSetLayout::BlockTexture>& DescriptorSetLayout::GetTextureBlocks() const
		{
			return _textureBlockVector;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool DescriptorSetLayout::BuildDescriptorSetLayout()
		{
			std::vector<VkDescriptorSetLayoutBinding> descriptors;

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
				textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
				OUTPUT_ERROR("Vulkan: to create descriptor set layout!\n");
				return false;
			}

			return true;
		}
	}
}
