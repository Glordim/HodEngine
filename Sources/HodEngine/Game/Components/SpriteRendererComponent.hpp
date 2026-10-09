#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Sprite.hpp"

namespace hod::inline renderer { class MaterialInstance; }

namespace hod::inline game
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API SpriteRendererComponent : public RendererComponent
	{
		REFLECTED_CLASS(SpriteRendererComponent, RendererComponent)

	public:

										SpriteRendererComponent() = default;
										SpriteRendererComponent(const SpriteRendererComponent&) = delete;
										SpriteRendererComponent(SpriteRendererComponent&&) = delete;
										~SpriteRendererComponent() override;

		SpriteRendererComponent&		operator=(const SpriteRendererComponent&) = delete;
		SpriteRendererComponent&		operator=(SpriteRendererComponent&&) = delete;

	public:
		/*
		const Sprite*			GetSprite() const;
		void							SetSprite(std::shared_ptr<Sprite> sprite);
		*/
		MaterialInstance*		GetMaterialInstance() const;
		void							SetMaterialInstance(MaterialInstance* materialInstance);

		void							PushRenderCommand(RenderView& renderView) override;
		Rect							GetBoundingBox() const override;

	private:

		WeakResource<Sprite>		_sprite;
		MaterialInstance*			_materialInstance = nullptr;
	};
}
