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

	/// @brief Fills the set and constant descriptors from the reflection emitted by the shader compiler.
	/// Only the binding kinds of the Vulkan target are handled for now.
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
				if (kind == "pushConstantBuffer")
				{
					const DocumentNode* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
					Assert(elementVarLayoutNode);
					bindingNode = elementVarLayoutNode->GetChild("binding");
					Assert(bindingNode);
					const DocumentNode* sizeNode = bindingNode->GetChild("size");
					Assert(sizeNode);
					_constantDescriptor = DefaultAllocator::GetInstance().New<ShaderConstantDescriptor>(0, sizeNode->GetUInt32(), GetShaderType());
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
						setDescriptor->ExtractBlockUbo(*parameterNode);
					}
					else if (kind == "resource")
					{
						const DocumentNode* baseShapeNode = typeNode->GetChild("baseShape");
						Assert(baseShapeNode);
						Assert(baseShapeNode->GetString() == "texture2D");
						setDescriptor->ExtractBlockTexture(*parameterNode);
					}
					else if (kind == "samplerState")
					{
						setDescriptor->ExtractBlockSampler(*parameterNode);
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

		return true;
	}
}
