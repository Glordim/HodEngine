#pragma once

#include "HodEngine/Game/Components/RendererComponent.h"

namespace hod
{
	namespace renderer
	{
		class Sprite;
		class MaterialInstance;
	}

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class SpriteComponent : public RendererComponent
		{
			REFLECTED_CLASS(SpriteComponent, RendererComponent)

		public:

											SpriteComponent() = default;
											SpriteComponent(const SpriteComponent&) = delete;
											SpriteComponent(SpriteComponent&&) = delete;
											~SpriteComponent() override;

			void							operator=(const SpriteComponent&) = delete;
			void							operator=(SpriteComponent&&) = delete;

		public:

			const char*						GetType() const override;

		public:

			const renderer::Sprite*			GetSprite() const;
			void							SetSprite(const renderer::Sprite* sprite);

			renderer::MaterialInstance*		GetMaterialInstance() const;
			void							SetMaterialInstance(renderer::MaterialInstance* materialInstance);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;

		private:

			const renderer::Sprite*			_sprite = nullptr;
			renderer::MaterialInstance*		_materialInstance = nullptr;
		};
	}
}
