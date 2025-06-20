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
		AddPropertyT(reflectionDescriptor, &MaterialResource::_vertexShader, "_vertexShader");
		AddPropertyT(reflectionDescriptor, &MaterialResource::_fragmentShader, "_fragmentShader");

		AddPropertyT(reflectionDescriptor, &MaterialResource::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialResource::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialResource::_defaultInstanceParams, "_defaultInstanceParams");
	}

	/// @brief 
	/// @return 
	MaterialResource::~MaterialResource()
	{
		DefaultAllocator::GetInstance().Delete(_material);
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool MaterialResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		Vector<VertexInput> vertexInputs;
		vertexInputs.push_back(VertexInput(0, 0, VertexInput::Format::R32G32_SFloat));
		vertexInputs.push_back(VertexInput(1, 8, VertexInput::Format::R32G32_SFloat));

		std::shared_ptr<ShaderResource> vertexShaderResource = _vertexShader.Lock();
		if (vertexShaderResource == nullptr)
		{
			return false;
		}
		Shader* vertexShader = vertexShaderResource->GetShader();
		if (vertexShader == nullptr)
		{
			return false;
		}

		std::shared_ptr<ShaderResource> fragmentShaderResource = _fragmentShader.Lock();
		if (fragmentShaderResource == nullptr)
		{
			return false;
		}
		Shader* fragmentShader = fragmentShaderResource->GetShader();
		if (fragmentShader == nullptr)
		{
			return false;
		}

		_material = Renderer::GetInstance()->CreateMaterial(vertexInputs.data(), (uint32_t)vertexInputs.size(), vertexShader, fragmentShader, _polygonMode, _topololy, false);
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
