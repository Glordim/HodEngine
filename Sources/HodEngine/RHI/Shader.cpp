#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	namespace
	{
		/// @brief Tells if a parameter of the reflection has the given attribute of ShaderLib/Attributes.slang
		bool HasUserAttribute(const DocumentNode& parameterNode, const char* attributeName)
		{
			const DocumentNode* userAttribsNode = parameterNode.GetChild("userAttribs");
			if (userAttribsNode == nullptr)
			{
				return false;
			}

			for (const DocumentNode* attributeNode = userAttribsNode->GetFirstChild(); attributeNode != nullptr; attributeNode = attributeNode->GetNextSibling())
			{
				const DocumentNode* nameNode = attributeNode->GetChild("name");
				if (nameNode != nullptr && nameNode->GetString() == attributeName)
				{
					return true;
				}
			}
			return false;
		}

		/// @brief The index of the binding of the given kind, among the one or several a variable of the reflection has. 0 if it has none.
		uint32_t GetBindingIndex(const DocumentNode& variableNode, const String& kind)
		{
			auto getIndex = [&kind](const DocumentNode& bindingNode, uint32_t& index) {
				const DocumentNode* kindNode = bindingNode.GetChild("kind");
				const DocumentNode* indexNode = bindingNode.GetChild("index");
				if (kindNode == nullptr || indexNode == nullptr || kindNode->GetString() != kind)
				{
					return false;
				}
				index = indexNode->GetUInt32();
				return true;
			};

			uint32_t index = 0;

			const DocumentNode* bindingNode = variableNode.GetChild("binding");
			if (bindingNode != nullptr && getIndex(*bindingNode, index))
			{
				return index;
			}

			const DocumentNode* bindingsNode = variableNode.GetChild("bindings");
			if (bindingsNode != nullptr)
			{
				for (bindingNode = bindingsNode->GetFirstChild(); bindingNode != nullptr; bindingNode = bindingNode->GetNextSibling())
				{
					if (getIndex(*bindingNode, index))
					{
						return index;
					}
				}
			}

			return 0;
		}
	}

	/// @brief 
	/// @param type 
	Shader::Shader(ShaderType type)
	{
		_type = type;
		_constantDescriptor = nullptr;
	}

	/// @brief 
	Shader::~Shader()
	{
		DefaultAllocator::GetInstance().Delete(_constantDescriptor);

		for (const auto& pair : _setDescriptors)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief 
	/// @return 
	const Vector<uint8_t>& Shader::GetShaderBytecode() const
	{
		return _buffer;
	}

	/// @brief 
	/// @return 
	Shader::ShaderType Shader::GetShaderType() const
	{
		return _type;
	}

	/// @brief 
	/// @return 
	const std::map<uint32_t, ShaderSetDescriptor*>& Shader::GetSetDescriptors() const
	{
		return _setDescriptors;
	}

	/// @brief 
	/// @return 
	const ShaderConstantDescriptor* Shader::GetConstantDescriptor() const
	{
		return _constantDescriptor;
	}

	/// @brief
	/// @param set
	/// @return
	ShaderSetDescriptor* Shader::GetOrCreateSetDescriptor(uint32_t set)
	{
		ShaderSetDescriptor* setDescriptor = nullptr;

		auto it = _setDescriptors.find(set);
		if (it != _setDescriptors.end())
		{
			setDescriptor = it->second;
		}
		else
		{
			setDescriptor = DefaultAllocator::GetInstance().New<ShaderSetDescriptor>();
			_setDescriptors.emplace(set, setDescriptor);
		}

		return setDescriptor;
	}

	/// @brief One set, made of the fields of a ParameterBlock: its ConstantBuffers, textures and samplers
	/// @param parameterNode
	/// @param space where the compiler put the block: the descriptor set on Vulkan, the register space on D3D12, the buffer index on Metal
	/// @return
	bool Shader::ExtractParameterBlock(const DocumentNode& parameterNode, uint32_t space)
	{
		ShaderSetDescriptor* setDescriptor = GetOrCreateSetDescriptor(space);

		const DocumentNode* nameNode = parameterNode.GetChild("name");
		const DocumentNode* typeNode = parameterNode.GetChild("type");
		const DocumentNode* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
		Assert(nameNode);
		Assert(elementVarLayoutNode);
		const DocumentNode* elementTypeNode = elementVarLayoutNode->GetChild("type");
		Assert(elementTypeNode);

		const DocumentNode* fieldsNode = elementTypeNode->GetChild("fields");
		if (fieldsNode == nullptr)
		{
			return true;
		}

		for (const DocumentNode* fieldNode = fieldsNode->GetFirstChild(); fieldNode != nullptr; fieldNode = fieldNode->GetNextSibling())
		{
			const DocumentNode* fieldNameNode = fieldNode->GetChild("name");
			const DocumentNode* fieldTypeNode = fieldNode->GetChild("type");
			const DocumentNode* fieldBindingNode = fieldNode->GetChild("binding");
			Assert(fieldNameNode);
			Assert(fieldTypeNode);

			const String&       fieldKind = fieldTypeNode->GetChild("kind")->GetString();
			const DocumentNode* baseShapeNode = fieldTypeNode->GetChild("baseShape");
			bool                isTexture = fieldKind == "resource" && baseShapeNode != nullptr && baseShapeNode->GetString() == "texture2D";
			if (fieldBindingNode == nullptr || (fieldKind != "constantBuffer" && fieldKind != "samplerState" && isTexture == false))
			{
				// Plain data would land in a constant buffer Vulkan and D3D12 add to the block, but Metal doesn't: the blocks would differ between targets
				OUTPUT_ERROR("Shader: \"{}.{}\" is not supported in a ParameterBlock, which takes ConstantBuffer, Texture2D and SamplerState fields", nameNode->GetString(),
				             fieldNameNode->GetString());
				return false;
			}

			// The binding of a field is relative to where the fields of its kind start in the block.
			// Metal has no binding: a field is known by its offset in the argument buffer of the block.
			uint32_t            binding = 0;
			const DocumentNode* fieldIndexNode = fieldBindingNode->GetChild("index");
			const DocumentNode* fieldOffsetNode = fieldBindingNode->GetChild("offset");
			if (fieldIndexNode != nullptr)
			{
				binding = fieldIndexNode->GetUInt32() + GetBindingIndex(*elementVarLayoutNode, fieldBindingNode->GetChild("kind")->GetString());
			}
			else if (fieldOffsetNode != nullptr)
			{
				binding = fieldOffsetNode->GetUInt32();
			}

			if (fieldKind == "constantBuffer")
			{
				setDescriptor->ExtractBlockUbo(*fieldNode, binding);
			}
			else if (isTexture)
			{
				setDescriptor->ExtractBlockTexture(*fieldNode, binding);
			}
			else
			{
				setDescriptor->ExtractBlockSampler(*fieldNode, binding);
			}
		}

		return true;
	}

	/// @brief Fills the set and constant descriptors from the reflection emitted by the shader compiler.
	/// A set is a ParameterBlock. The parameters declared outside of any block make one more set, which comes first.
	/// The sets are numbered from 0 in the order the compiler placed them, whatever the place itself (see ShaderSetDescriptor::GetSpace):
	/// on D3D12 the push constant takes the first register space when there is nothing else outside of the blocks, which then start at the second.
	/// @return
	bool Shader::GenerateDescriptors(const char* reflection, uint32_t reflectionSize)
	{
		Document           reflectionDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(reflectionDocument, reflection, reflectionSize) == false)
		{
			return false;
		}

		const DocumentNode* parametersNode = reflectionDocument.GetRootNode().GetChild("parameters");
		if (parametersNode)
		{
			const DocumentNode* parameterNode = parametersNode->GetFirstChild();
			while (parameterNode != nullptr)
			{
				const DocumentNode* nameNode = parameterNode->GetChild("name");
				const DocumentNode* bindingNode = parameterNode->GetChild("binding");
				const DocumentNode* typeNode = parameterNode->GetChild("type");
				Assert(nameNode);
				Assert(bindingNode);
				Assert(typeNode);

				const DocumentNode* kindNode = bindingNode->GetChild("kind");
				const DocumentNode* indexNode = bindingNode->GetChild("index");
				Assert(kindNode);
				Assert(indexNode);

				const String& kind = kindNode->GetString();
				if (typeNode->GetChild("kind")->GetString() == "parameterBlock")
				{
					if (ExtractParameterBlock(*parameterNode, indexNode->GetUInt32()) == false)
					{
						return false;
					}
				}
				// Only Vulkan has a kind for [[vk::push_constant]]: on the other targets it is a constant buffer, known by its [PushConstant] attribute
				else if (kind == "pushConstantBuffer" || (kind == "constantBuffer" && HasUserAttribute(*parameterNode, "PushConstant")))
				{
					const DocumentNode* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
					Assert(elementVarLayoutNode);
					const DocumentNode* elementBindingNode = elementVarLayoutNode->GetChild("binding");
					Assert(elementBindingNode);
					const DocumentNode* sizeNode = elementBindingNode->GetChild("size");
					Assert(sizeNode);
					_constantDescriptor =
						DefaultAllocator::GetInstance().New<ShaderConstantDescriptor>(0, sizeNode->GetUInt32(), indexNode->GetUInt32(), GetShaderType());
				}
				// Vulkan numbers everything in one space per set ("descriptorTableSlot"),
				// Metal and D3D12 number buffers, textures and samplers separately
				else if (kind == "descriptorTableSlot" || kind == "constantBuffer" || kind == "shaderResource" || kind == "samplerState")
				{
					uint32_t              set = 0;
					const DocumentNode* spaceNode = bindingNode->GetChild("space");
					if (spaceNode != nullptr)
					{
						set = spaceNode->GetUInt32();
					}
					ShaderSetDescriptor* setDescriptor = GetOrCreateSetDescriptor(set);

					kindNode = typeNode->GetChild("kind");
					const String& kind = kindNode->GetString();
					if (kind == "constantBuffer")
					{
						setDescriptor->ExtractBlockUbo(*parameterNode, indexNode->GetUInt32());
					}
					else if (kind == "resource")
					{
						const DocumentNode* baseShapeNode = typeNode->GetChild("baseShape");
						Assert(baseShapeNode);
						Assert(baseShapeNode->GetString() == "texture2D");
						setDescriptor->ExtractBlockTexture(*parameterNode, indexNode->GetUInt32());
					}
					else if (kind == "samplerState")
					{
						setDescriptor->ExtractBlockSampler(*parameterNode, indexNode->GetUInt32());
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

				parameterNode = parameterNode->GetNextSibling();
			}
		}

		// So far the sets are keyed by where the compiler put them
		std::map<uint32_t, ShaderSetDescriptor*> setDescriptors;
		for (const auto& pair : _setDescriptors)
		{
			pair.second->SetSpace(pair.first);
			setDescriptors.emplace((uint32_t)setDescriptors.size(), pair.second);
		}
		_setDescriptors = std::move(setDescriptors);

		return true;
	}
}
