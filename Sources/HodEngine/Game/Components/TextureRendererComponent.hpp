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

		void							OnConstruct() override;

		renderer::MaterialInstance*		GetMaterialInstance() const;

		void							SetMaterialInstanceResource(const WeakResource<renderer::MaterialInstanceResource>& materialInstance);

		void							SetTexture(const WeakResource<renderer::TextureResource>& texture);

		void							PushRenderCommand(renderer::RenderView& renderView) override;

		Rect							GetBoundingBox() const override;

		void							SetColor(const Color& color);
		const Color&					GetColor() const;

	private:

		void							RefreshMaterialInstance();

	private:

		WeakResource<renderer::TextureResource>				_texture;
		WeakResource<renderer::MaterialInstanceResource>	_material;
		float												_pixelPerUnit = 100.0f;

		Color												_color;
		
		renderer::MaterialInstance*							_materialInstance = nullptr;
		renderer::MaterialInstance*							_builtinMaterialInstance = nullptr;
	};
}
