#pragma once

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/WeakResource.hpp"

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
		class SpriteRendererComponent : public RendererComponent
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
			const renderer::Sprite*			GetSprite() const;
			void							SetSprite(std::shared_ptr<renderer::Sprite> sprite);
			*/
			renderer::MaterialInstance*		GetMaterialInstance() const;
			void							SetMaterialInstance(renderer::MaterialInstance* materialInstance);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;

		private:

			WeakResource<renderer::Sprite>		_sprite;
			renderer::MaterialInstance*			_materialInstance = nullptr;
		};
	}
}
