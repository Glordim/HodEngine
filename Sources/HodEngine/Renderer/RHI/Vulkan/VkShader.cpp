#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <cstring>
#include <string_view>

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderConstantDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>

#undef min
#undef max

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
	bool VkShader::LoadFromIR(const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.codeSize = bytecodeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode);

		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		if (vkCreateShaderModule(renderer->GetVkDevice(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		{
			OUTPUT_ERROR("VkShader : Failed to create Shader Module");
			return false;
		}

		_buffer.Resize(bytecodeSize);
		memcpy(_buffer.Data(), bytecode, bytecodeSize);

		if (GenerateDescriptors(reflection, reflectionSize) == false)
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
	bool VkShader::GenerateDescriptors(const char* reflection, uint32_t reflectionSize)
	{
		Document           reflectionDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(reflectionDocument, reflection, reflectionSize) == false)
		{
			return false;
		}

		const Document::Node* parametersNode = reflectionDocument.GetRootNode().GetChild("parameters");
		if (parametersNode)
		{
			const Document::Node* parameterNode = parametersNode->GetFirstChild();
			while (parameterNode != nullptr)
			{
				const Document::Node* nameNode = parameterNode->GetChild("name");
				const Document::Node* bindingNode = parameterNode->GetChild("binding");
				const Document::Node* typeNode = parameterNode->GetChild("type");
				Assert(nameNode);
				Assert(bindingNode);
				Assert(typeNode);

				const Document::Node* kindNode = bindingNode->GetChild("kind");
				const Document::Node* indexNode = bindingNode->GetChild("index");
				Assert(kindNode);
				Assert(indexNode);

				const String& kind = kindNode->GetString();
				if (kind == "pushConstantBuffer")
				{
					const Document::Node* elementVarLayoutNode = typeNode->GetChild("elementVarLayout");
					Assert(elementVarLayoutNode);
					bindingNode = elementVarLayoutNode->GetChild("binding");
					Assert(bindingNode);
					const Document::Node* sizeNode = bindingNode->GetChild("size");
					Assert(sizeNode);
					_constantDescriptor = DefaultAllocator::GetInstance().New<ShaderConstantDescriptorVk>(0, sizeNode->GetUInt32(), GetShaderType());
				}
				else if (kind == "descriptorTableSlot")
				{
					uint32_t              set = 0;
					const Document::Node* spaceNode = bindingNode->GetChild("space");
					if (spaceNode != nullptr)
					{
						set = spaceNode->GetUInt32();
					}
					ShaderSetDescriptorVk* setDescriptor = GetOrCreateSetDescriptor(set);

					kindNode = typeNode->GetChild("kind");
					const String& kind = kindNode->GetString();
					if (kind == "constantBuffer")
					{
						setDescriptor->ExtractBlockUbo(*parameterNode);
					}
					else if (kind == "resource")
					{
						const Document::Node* baseShapeNode = typeNode->GetChild("baseShape");
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
