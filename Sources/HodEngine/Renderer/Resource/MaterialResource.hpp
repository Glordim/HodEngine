#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource/Resource.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"

#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::renderer
{
	class TextureResource;

	class HOD_RENDERER_API MaterialResource : public Resource
	{
		REFLECTED_CLASS(MaterialResource, Resource)

	public:

							MaterialResource() = default;
							MaterialResource(const MaterialResource&) = delete;
							MaterialResource(MaterialResource&&) = delete;
							~MaterialResource() override;

		MaterialResource&	operator = (const MaterialResource&) = delete;
		MaterialResource&	operator = (MaterialResource&&) = delete;

	public:

		bool				Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas) override;

		Material*			GetMaterial() const;

	private:

		Material*			_material = nullptr;

		Shader*				_vertexShader = nullptr;
		Shader*				_fragmentShader = nullptr;

		Material::PolygonMode	_polygonMode = Material::PolygonMode::Fill;
		Material::Topololy		_topololy = Material::Topololy::TRIANGLE;

		Document									_defaultInstanceParams;
		Vector<WeakResource<TextureResource>>	_textureResources;
	};
}