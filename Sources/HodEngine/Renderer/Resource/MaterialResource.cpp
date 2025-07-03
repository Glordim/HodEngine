#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
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

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool MaterialResource::Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		if (datas.size() != 2)
		{
			// TODO message
			return false;
		}

		const Resource::Data& vertexData = datas[0];
		_vertexShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Vertex);
		if (_vertexShader->LoadFromIR(vertexData._buffer, vertexData._size) == false)
		{
			DefaultAllocator::GetInstance().Delete(_vertexShader);
			_vertexShader = nullptr;
			return false;
		}

		const Resource::Data& fragmentData = datas[1];
		_fragmentShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Fragment);
		if (_fragmentShader->LoadFromIR(fragmentData._buffer, fragmentData._size) == false)
		{
			DefaultAllocator::GetInstance().Delete(_fragmentShader);
			_fragmentShader = nullptr;
			return false;
		}

		Vector<VertexInput> vertexInputs;
		vertexInputs.push_back(VertexInput(0, 0, VertexInput::Format::R32G32_SFloat));
		vertexInputs.push_back(VertexInput(1, 8, VertexInput::Format::R32G32_SFloat));

		_material = Renderer::GetInstance()->CreateMaterial(vertexInputs.data(), (uint32_t)vertexInputs.size(), _vertexShader, _fragmentShader, _polygonMode, _topololy, false);
		if (_material == nullptr)
		{
			return false;
		}

		_material->CreateDefaultInstance();
		MaterialSerializationHelper::ApplyParamsFromDocument(*const_cast<MaterialInstance*>(_material->GetDefaultInstance()), _defaultInstanceParams.GetRootNode(), _textureResources);

		return true;
	}

	/// @brief 
	/// @return 
	Material* MaterialResource::GetMaterial() const
	{
		return _material;
	}
}
