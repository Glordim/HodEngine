#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	void ShaderSetDescriptor::Merge(const ShaderSetDescriptor& other)
	{
		for (const BlockUbo& otherUboBlock : other._uboBlockVector)
		{
			bool alreadyExist = false;
			for (BlockUbo& exisintgUboBlock : _uboBlockVector)
			{
				if (otherUboBlock._binding == exisintgUboBlock._binding) // Todo binding is enought or check name too can be useful ?
				{
					alreadyExist = true;
					break;
				}
			}
			if (alreadyExist == false)
			{
				_uboBlockVector.push_back(otherUboBlock);
			}
		}

		for (const BlockTexture& otherTextureBlock : other._textureBlockVector)
		{
			bool alreadyExist = false;
			for (BlockTexture& exisintgTextureBlock : _textureBlockVector)
			{
				// A texture and a sampler can share the same binding index on the targets numbering them separately
				if (otherTextureBlock._binding == exisintgTextureBlock._binding && otherTextureBlock._type == exisintgTextureBlock._type)
				{
					alreadyExist = true;
					break;
				}
			}
			if (alreadyExist == false)
			{
				_textureBlockVector.push_back(otherTextureBlock);
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const Vector<ShaderSetDescriptor::BlockUbo>& ShaderSetDescriptor::GetUboBlocks() const
	{
		return _uboBlockVector;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const Vector<ShaderSetDescriptor::BlockTexture>& ShaderSetDescriptor::GetTextureBlocks() const
	{
		return _textureBlockVector;
	}

	/// @brief
	/// @param comp
	/// @param resource
	void ShaderSetDescriptor::ExtractBlockUbo(const DocumentNode& parameterNode)
	{
		const DocumentNode* nameNode = parameterNode.GetChild("name");
		const DocumentNode* bindingNode = parameterNode.GetChild("binding");
		const DocumentNode* indexNode = bindingNode->GetChild("index");
		const DocumentNode* typeNode = parameterNode.GetChild("type");
		Assert(typeNode);
		const DocumentNode* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
		Assert(elementVarLayoutNode);
		bindingNode = elementVarLayoutNode->GetChild("binding");
		Assert(bindingNode);
		const DocumentNode* sizeNode = bindingNode->GetChild("size");
		Assert(sizeNode);
		typeNode = elementVarLayoutNode->GetChild("type");
		Assert(typeNode);
		const DocumentNode* kindNode = typeNode->GetChild("kind");
		Assert(kindNode);
		Assert(kindNode->GetString() == "struct");
		const DocumentNode* fieldsNode = typeNode->GetChild("fields");
		Assert(fieldsNode);

		BlockUbo ubo;
		ubo._binding = indexNode->GetUInt32();
		ubo._name = nameNode->GetString();
		ubo._rootMember._name = ubo._name;
		ubo._rootMember._offset = 0;
		ubo._rootMember._size = sizeNode->GetUInt32();
		ubo._rootMember._count = 1; // todo array, ex: ConstantBuffer<float4> test[4];

		const DocumentNode* fieldNode = fieldsNode->GetFirstChild();
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
	void ShaderSetDescriptor::ExtractUboSubMembers(const DocumentNode& fieldNode, BlockUbo::Member& structMember)
	{
		const DocumentNode* nameNode = fieldNode.GetChild("name");
		const DocumentNode* bindingNode = fieldNode.GetChild("binding");
		const DocumentNode* typeNode = fieldNode.GetChild("type");
		Assert(nameNode);
		Assert(bindingNode);
		Assert(typeNode);

		const DocumentNode* offsetNode = bindingNode->GetChild("offset");
		const DocumentNode* sizeNode = bindingNode->GetChild("size");
		Assert(offsetNode);
		Assert(sizeNode);

		const DocumentNode* kindNode = typeNode->GetChild("kind");
		Assert(kindNode);

		BlockUbo::Member member;
		member._name = nameNode->GetString();
		member._offset = offsetNode->GetUInt32();
		member._size = sizeNode->GetUInt32();
		const String& kind = kindNode->GetString();
		if (kind == "scalar")
		{
			const DocumentNode* scalarTypeNode = typeNode->GetChild("scalarType");
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
			const DocumentNode* elementCountNode = typeNode->GetChild("elementCount");
			Assert(elementCountNode);
			member._count = elementCountNode->GetUInt32();

			const DocumentNode* elementTypeNode = typeNode->GetChild("elementType");
			Assert(elementTypeNode);

			kindNode = elementTypeNode->GetChild("kind");
			Assert(kindNode);
			const String& kind = kindNode->GetString();

			if (kind == "scalar")
			{
				const DocumentNode* scalarTypeNode = elementTypeNode->GetChild("scalarType");
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
			const DocumentNode* fieldsNode = typeNode->GetChild("fields");
			Assert(fieldsNode);
			const DocumentNode* fieldNode = fieldsNode->GetFirstChild();
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
	void ShaderSetDescriptor::ExtractBlockTexture(const DocumentNode& parameterNode)
	{
		const DocumentNode* nameNode = parameterNode.GetChild("name");
		const DocumentNode* bindingNode = parameterNode.GetChild("binding");
		const DocumentNode* indexNode = bindingNode->GetChild("index");

		BlockTexture texture;
		texture._type = BlockTexture::Type::Texture;
		texture._binding = indexNode->GetUInt32();
		texture._name = nameNode->GetString();

		_textureBlockVector.PushBack(std::move(texture));
	}

	void ShaderSetDescriptor::ExtractBlockSampler(const DocumentNode& parameterNode)
	{
		const DocumentNode* nameNode = parameterNode.GetChild("name");
		const DocumentNode* bindingNode = parameterNode.GetChild("binding");
		const DocumentNode* indexNode = bindingNode->GetChild("index");

		BlockTexture texture;
		texture._type = BlockTexture::Type::Sampler;
		texture._binding = indexNode->GetUInt32();
		texture._name = nameNode->GetString();

		_textureBlockVector.PushBack(std::move(texture));
	}
}
