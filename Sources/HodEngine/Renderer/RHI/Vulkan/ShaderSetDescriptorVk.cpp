#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Core/Assert.hpp>

#undef min
#undef max

namespace hod::renderer
{
	/// @brief
	/// @param type
	/// @return
	VkDescriptorType ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(BlockTexture::Type type)
	{
		switch (type)
		{
			case BlockTexture::Type::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;

			case BlockTexture::Type::Texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

			case BlockTexture::Type::Combined: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
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
	void ShaderSetDescriptorVk::ExtractBlockUbo(const Document::Node& parameterNode)
	{
		const Document::Node* nameNode = parameterNode.GetChild("name");
		const Document::Node* bindingNode = parameterNode.GetChild("binding");
		const Document::Node* indexNode = bindingNode->GetChild("index");
		const Document::Node* typeNode = parameterNode.GetChild("type");
		Assert(typeNode);
		const Document::Node* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
		Assert(elementVarLayoutNode);
		bindingNode = elementVarLayoutNode->GetChild("binding");
		Assert(bindingNode);
		const Document::Node* sizeNode = bindingNode->GetChild("size");
		Assert(sizeNode);
		typeNode = elementVarLayoutNode->GetChild("type");
		Assert(typeNode);
		const Document::Node* kindNode = typeNode->GetChild("kind");
		Assert(kindNode);
		Assert(kindNode->GetString() == "struct");
		const Document::Node* fieldsNode = typeNode->GetChild("fields");
		Assert(fieldsNode);

		BlockUbo ubo;
		ubo._binding = indexNode->GetUInt32();
		ubo._name = nameNode->GetString();
		ubo._rootMember._name = ubo._name;
		ubo._rootMember._offset = 0;
		ubo._rootMember._size = sizeNode->GetUInt32();
		ubo._rootMember._count = 1; // todo array, ex: ConstantBuffer<float4> test[4];

		const Document::Node* fieldNode = fieldsNode->GetFirstChild();
		while (fieldNode != nullptr)
		{
			ExtractUboSubMembers(*fieldNode, ubo._rootMember);
			fieldNode = fieldNode->GetNextSibling();
		}

		_uboBlockVector.PushBack(std::move(ubo));
	}

	/// @brief
	/// @param comp
	/// @param structType
	/// @param structMember
	void ShaderSetDescriptorVk::ExtractUboSubMembers(const Document::Node& fieldNode, BlockUbo::Member& structMember)
	{
		const Document::Node* nameNode = fieldNode.GetChild("name");
		const Document::Node* bindingNode = fieldNode.GetChild("binding");
		const Document::Node* typeNode = fieldNode.GetChild("type");
		Assert(nameNode);
		Assert(bindingNode);
		Assert(typeNode);

		const Document::Node* offsetNode = bindingNode->GetChild("offset");
		const Document::Node* sizeNode = bindingNode->GetChild("size");
		Assert(offsetNode);
		Assert(sizeNode);

		const Document::Node* kindNode = typeNode->GetChild("kind");
		Assert(kindNode);

		BlockUbo::Member member;
		member._name = nameNode->GetString();
		member._offset = offsetNode->GetUInt32();
		member._size = sizeNode->GetUInt32();
		const String& kind = kindNode->GetString();
		if (kind == "scalar")
		{
			const Document::Node* scalarTypeNode = typeNode->GetChild("scalarType");
			Assert(scalarTypeNode);
			const String& scalarType = scalarTypeNode->GetString();
			if (scalarType == "float32")
			{
				member._memberType = BlockUbo::MemberType::Float;
			}
			else
			{
				Assert(false);
			}
		}
		else if (kind == "vector")
		{
			const Document::Node* elementCountNode = typeNode->GetChild("elementCount");
			Assert(elementCountNode);
			member._count = elementCountNode->GetUInt32();

			const Document::Node* elementTypeNode = typeNode->GetChild("elementType");
			Assert(elementTypeNode);

			kindNode = elementTypeNode->GetChild("kind");
			Assert(kindNode);
			const String& kind = kindNode->GetString();

			if (kind == "scalar")
			{
				const Document::Node* scalarTypeNode = elementTypeNode->GetChild("scalarType");
				Assert(scalarTypeNode);

				const String& scalarType = scalarTypeNode->GetString();
				if (scalarType == "float32")
				{
					if (member._count == 2)
					{
						member._memberType = BlockUbo::MemberType::Float2;
					}
					else if (member._count == 4)
					{
						member._memberType = BlockUbo::MemberType::Float4;
					}
					else
					{
						Assert(false);
					}
				}
				else
				{
					Assert(false);
				}
			}
			else
			{
				Assert(false);
			}
		}
		else if (kind == "struct")
		{
			const Document::Node* fieldsNode = typeNode->GetChild("fields");
			Assert(fieldsNode);
			const Document::Node* fieldNode = fieldsNode->GetFirstChild();
			while (fieldNode != nullptr)
			{
				ExtractUboSubMembers(*fieldNode, member);
				fieldNode = fieldNode->GetNextSibling();
			}
		}
		structMember._childsMap.emplace(member._name, std::move(member));
	}

	/// @brief
	/// @param comp
	/// @param resource
	/// @param type
	void ShaderSetDescriptorVk::ExtractBlockTexture(const Document::Node& parameterNode)
	{
		const Document::Node* nameNode = parameterNode.GetChild("name");
		const Document::Node* bindingNode = parameterNode.GetChild("binding");
		const Document::Node* indexNode = bindingNode->GetChild("index");

		BlockTexture texture;
		texture._type = BlockTexture::Type::Texture;
		texture._binding = indexNode->GetUInt32();
		texture._name = nameNode->GetString();

		_textureBlockVector.PushBack(std::move(texture));
	}

	void ShaderSetDescriptorVk::ExtractBlockSampler(const Document::Node& parameterNode)
	{
		const Document::Node* nameNode = parameterNode.GetChild("name");
		const Document::Node* bindingNode = parameterNode.GetChild("binding");
		const Document::Node* indexNode = bindingNode->GetChild("index");

		BlockTexture texture;
		texture._type = BlockTexture::Type::Sampler;
		texture._binding = indexNode->GetUInt32();
		texture._name = nameNode->GetString();

		_textureBlockVector.PushBack(std::move(texture));
	}

	/// @brief
	/// @return
	bool ShaderSetDescriptorVk::BuildDescriptorSetLayout()
	{
		Vector<VkDescriptorSetLayoutBinding> descriptors;

		size_t uboCount = _uboBlockVector.Size();
		for (size_t i = 0; i < uboCount; ++i)
		{
			BlockUbo& ubo = _uboBlockVector[i];

			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = ubo._binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.PushBack(std::move(uboLayoutBinding));
		}

		size_t textureCount = _textureBlockVector.Size();
		for (size_t i = 0; i < textureCount; ++i)
		{
			BlockTexture& texture = _textureBlockVector[i];

			VkDescriptorSetLayoutBinding textureLayoutBinding = {};
			textureLayoutBinding.binding = texture._binding;
			textureLayoutBinding.descriptorType = ShaderSetDescriptorVk::TextureTypeToVkDescriptorType(texture._type);
			textureLayoutBinding.descriptorCount = 1;
			textureLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			textureLayoutBinding.pImmutableSamplers = nullptr;

			descriptors.PushBack(std::move(textureLayoutBinding));
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)descriptors.Size();
		layoutInfo.pBindings = descriptors.Data();

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateDescriptorSetLayout(renderer->GetVkDevice(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: to create descriptor set layout!");
			return false;
		}

		return true;
	}
}
