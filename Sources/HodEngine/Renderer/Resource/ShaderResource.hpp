#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace hod::renderer
{	
	class HOD_RENDERER_API ShaderResource : public Resource
	{
		REFLECTED_CLASS(ShaderResource, Resource)

	public:

							ShaderResource() = default;
							ShaderResource(const ShaderResource&) = delete;
							ShaderResource(ShaderResource&&) = delete;
							~ShaderResource() override;

		ShaderResource&		operator = (const ShaderResource&) = delete;
		ShaderResource&		operator = (ShaderResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		Shader*				GetShader() const;

	private:

		Shader*				_shader = nullptr;

		Shader::ShaderType	_type;
		std::string			_source;
	};
}