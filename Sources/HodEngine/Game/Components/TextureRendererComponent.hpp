#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
#include <HodEngine/Math/Rect.hpp>

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline game
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

		MaterialInstance*		GetMaterialInstance() const;

		void							SetMaterialInstanceResource(const WeakResource<MaterialInstanceResource>& materialInstance);

		void							SetTexture(const WeakResource<TextureResource>& texture);

		void							PushRenderCommand(RenderView& renderView) override;

		Rect							GetBoundingBox() const override;

		void							SetColor(const Color& color);
		const Color&					GetColor() const;

	private:

		void							RefreshMaterialInstance();

	private:

		WeakResource<TextureResource>				_texture;
		WeakResource<MaterialInstanceResource>	_material;
		float												_pixelPerUnit = 100.0f;

		Color												_color;
		
		MaterialInstance*							_materialInstance = nullptr;
		MaterialInstance*							_builtinMaterialInstance = nullptr;
	};
}
