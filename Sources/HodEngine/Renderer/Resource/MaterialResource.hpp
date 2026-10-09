#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::inline rhi
{
	class MaterialInstance;
}

namespace hod::inline renderer
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

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		GraphicsPipeline*			GetMaterial() const;

		const MaterialInstance*	GetDefaultInstance() const;
		MaterialInstance*		EditDefaultInstance();

	private:

		GraphicsPipeline*			_material = nullptr;
		MaterialInstance*	_defaultInstance = nullptr;

		Shader*				_vertexShader = nullptr;
		Shader*				_fragmentShader = nullptr;

		GraphicsPipeline::PolygonMode	_polygonMode = GraphicsPipeline::PolygonMode::Fill;
		GraphicsPipeline::Topololy		_topololy = GraphicsPipeline::Topololy::TRIANGLE;

		Document									_defaultInstanceParams;
		Vector<WeakResource<TextureResource>>	_textureResources;
	};
}