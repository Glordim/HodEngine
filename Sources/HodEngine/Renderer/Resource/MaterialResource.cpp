#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(MaterialResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialResource::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialResource::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialResource::_defaultInstanceParams, "_defaultInstanceParams");
	}

	/// @brief
	/// @return
	MaterialResource::~MaterialResource()
	{
		DefaultAllocator::GetInstance().Delete(_material);
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
	}

	namespace
	{
		bool ReadDataBlock(const ResourceContainer& resourceContainer, std::string_view name, Vector<uint8_t>& outData)
		{
			const ResourceContainer::DataBlockInfo* dataBlock = resourceContainer.FindDataBlock(name);
			if (dataBlock == nullptr)
			{
				OUTPUT_ERROR("MaterialResource::Initialize: missing '{}' data block", name);
				return false;
			}

			outData.Resize(dataBlock->_uncompressedSize);
			if (dataBlock->_stream->Read(outData.Data(), outData.Size()) != outData.Size())
			{
				OUTPUT_ERROR("MaterialResource::Initialize: can't read '{}' data block", name);
				return false;
			}
			return true;
		}
	}

	bool MaterialResource::Initialize(const ResourceContainer& resourceContainer)
	{
		Vector<uint8_t> vertexData;
		Vector<uint8_t> vertexReflectionData;
		if (ReadDataBlock(resourceContainer, "Vertex", vertexData) == false || ReadDataBlock(resourceContainer, "VertexReflection", vertexReflectionData) == false)
		{
			return false;
		}

		_vertexShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Vertex);
		if (_vertexShader->LoadFromIR(vertexData.Data(), vertexData.Size(), reinterpret_cast<const char*>(vertexReflectionData.Data()), vertexReflectionData.Size()) == false)
		{
			OUTPUT_ERROR("MaterialResource::Initialize: load vertex shader failed");
			DefaultAllocator::GetInstance().Delete(_vertexShader);
			_vertexShader = nullptr;
			return false;
		}

		Vector<uint8_t> fragmentData;
		Vector<uint8_t> fragmentReflectionData;
		if (ReadDataBlock(resourceContainer, "Fragment", fragmentData) == false || ReadDataBlock(resourceContainer, "FragmentReflection", fragmentReflectionData) == false)
		{
			return false;
		}

		_fragmentShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Fragment);
		if (_fragmentShader->LoadFromIR(fragmentData.Data(), fragmentData.Size(), reinterpret_cast<const char*>(fragmentReflectionData.Data()), fragmentReflectionData.Size()) == false)
		{
			OUTPUT_ERROR("MaterialResource::Initialize: load fragment shader failed");
			DefaultAllocator::GetInstance().Delete(_fragmentShader);
			_fragmentShader = nullptr;
			return false;
		}

		Vector<VertexInput> vertexInputs;
		vertexInputs.PushBack(VertexInput(0, 0, VertexInput::Format::R32G32_SFloat));
		vertexInputs.PushBack(VertexInput(1, 8, VertexInput::Format::R32G32_SFloat));

		_material = Renderer::GetInstance()->CreateMaterial(vertexInputs.Data(), (uint32_t)vertexInputs.Size(), _vertexShader, _fragmentShader, _polygonMode, _topololy, false);
		if (_material == nullptr)
		{
			OUTPUT_ERROR("MaterialResource::Initialize: load material failed");
			return false;
		}

		_material->CreateDefaultInstance();
		MaterialSerializationHelper::ApplyParamsFromDocument(*const_cast<MaterialInstance*>(_material->GetDefaultInstance()), _defaultInstanceParams.GetRootNode(),
		                                                     _textureResources);

		return true;
	}

	/// @brief
	/// @return
	Material* MaterialResource::GetMaterial() const
	{
		return _material;
	}
}
