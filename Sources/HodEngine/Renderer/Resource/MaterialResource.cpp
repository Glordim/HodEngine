#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(MaterialResource, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &MaterialResource::_vertexShader, "_vertexShader");
		AddPropertyT(&reflectionDescriptor, &MaterialResource::_fragmentShader, "_fragmentShader");

		AddPropertyT(&reflectionDescriptor, &MaterialResource::_polygonMode, "_polygonMode");
		AddPropertyT(&reflectionDescriptor, &MaterialResource::_topololy, "_topololy");
	}

	/// @brief 
	/// @return 
	MaterialResource::~MaterialResource()
	{
		delete _material;
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

		const VertexInput* vertexInputs = nullptr;
		uint32_t vertexInputCount = 0;

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

		_material = Renderer::GetInstance()->CreateMaterial(vertexInputs, vertexInputCount,vertexShader, fragmentShader, _polygonMode, _topololy, false);
		if (_material == nullptr)
		{
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	Material* MaterialResource::GetMaterial() const
	{
		return _material;
	}
}
