#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/Renderer/Material.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::inline renderer
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

		Material*			GetMaterial() const;

		const MaterialInstance*	GetDefaultInstance() const;
		MaterialInstance*		EditDefaultInstance();

		// Live edit (material editor): rebuilds the pipeline of the material in place, the instances of it see the change.
		// Keeps the current states if the pipeline can't be built with them.
		bool				SetPipelineStates(Material::PolygonMode polygonMode, Material::Topology topology, BlendMode blendMode, CullMode cullMode, bool depthTest, bool depthWrite);

	private:

		Material::CreateInfo	MakeCreateInfo() const;

	private:

		Material*			_material = nullptr;
		MaterialInstance*	_defaultInstance = nullptr;

		Shader*				_vertexShader = nullptr;
		Shader*				_fragmentShader = nullptr;

		Material::PolygonMode	_polygonMode = Material::PolygonMode::Fill;
		Material::Topology		_topology = Material::Topology::TRIANGLE;
		BlendMode				_blendMode = BlendMode::Alpha;
		CullMode				_cullMode = CullMode::None;
		bool					_depthTest = false;
		bool					_depthWrite = false;

		Document									_defaultInstanceParams;
		Vector<WeakResource<TextureResource>>	_textureResources;
	};
}