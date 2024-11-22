#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/ShaderResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(ShaderResource, reflectionDescriptor)
	{
		AddPropertyT(&reflectionDescriptor, &ShaderResource::_type, "_type");
		AddPropertyT(&reflectionDescriptor, &ShaderResource::_source, "_source");
	}

	/// @brief 
	/// @return 
	ShaderResource::~ShaderResource()
	{
		delete _shader;
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool ShaderResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		_shader = Renderer::GetInstance()->CreateShader(_type);
		if (_shader->LoadFromSource(_source) == false)
		{
			delete _shader;
			_shader = nullptr;
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	Shader* ShaderResource::GetShader() const
	{
		return _shader;
	}
}
