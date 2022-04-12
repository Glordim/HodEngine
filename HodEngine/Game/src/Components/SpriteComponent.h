#pragma once

#include "RendererComponent.h"

namespace HOD
{
	namespace RENDERER
	{
		class Sprite;
		class MaterialInstance;
	}

	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class SpriteComponent : public RendererComponent
		{
			REFLECTED_DERIVED_CLASS(SpriteComponent, RendererComponent);

		public:

											SpriteComponent(Actor* actor);
											SpriteComponent(const SpriteComponent&) = delete;
											SpriteComponent(SpriteComponent&&) = delete;
											~SpriteComponent() override;

			void							operator=(const SpriteComponent&) = delete;
			void							operator=(SpriteComponent&&) = delete;

		public:

			void							DrawImGui() override;
			const char*						GetType() const override;

		public:

			const RENDERER::Sprite*			GetSprite() const;
			void							SetSprite(const RENDERER::Sprite* sprite);

			RENDERER::MaterialInstance*		GetMaterialInstance() const;
			void							SetMaterialInstance(RENDERER::MaterialInstance* materialInstance);

			void							PushToRenderQueue(RENDERER::RenderQueue& renderQueue) override;

		private:

			const RENDERER::Sprite*			_sprite = nullptr;
			RENDERER::MaterialInstance*		_materialInstance;
		};
	}
}
