#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
#include <HodEngine/Core/Rect.hpp>

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API TextureRendererComponent : public RendererComponent
	{
		REFLECTED_CLASS(TextureRendererComponent, RendererComponent)

	public:

										TextureRendererComponent();
										TextureRendererComponent(const TextureRendererComponent&) = delete;
										TextureRendererComponent(TextureRendererComponent&&) = delete;
										~TextureRendererComponent() override;

		TextureRendererComponent&		operator=(const TextureRendererComponent&) = delete;
		TextureRendererComponent&		operator=(TextureRendererComponent&&) = delete;

	public:

		renderer::MaterialInstance*		GetMaterialInstance() const;
		void							SetMaterialInstance(renderer::MaterialInstance* materialInstance);

		void							SetMaterialInstanceResource(const WeakResource<renderer::MaterialInstanceResource>& materialInstance);

		void							SetTexture(const WeakResource<renderer::TextureResource>& texture);

		void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;

		Rect							GetBoundingBox() const;

	private:

		void							RefreshMaterialInstance();

	private:

		WeakResource<renderer::TextureResource>				_texture;
		WeakResource<renderer::MaterialInstanceResource>	_material;
		float												_pixelPerUnit = 100.0f;
		
		renderer::MaterialInstance*							_materialInstance = nullptr;
		renderer::MaterialInstance*							_builtinMaterialInstance = nullptr;
	};
}
