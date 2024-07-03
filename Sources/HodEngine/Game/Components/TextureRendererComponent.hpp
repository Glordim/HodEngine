#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;
	}

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API TextureRendererComponent : public RendererComponent
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
			void							SetMaterialInstance(renderer::MaterialInstance* materialInstance);

			void							SetTexture(const WeakResource<renderer::TextureResource>& texture);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;
			void							PushPickingToRenderQueue(renderer::RenderQueue& renderQueue, const Color& colorId) override;

		private:

			WeakResource<renderer::TextureResource>	_textureResource;

			
			renderer::MaterialInstance*				_materialInstance = nullptr;
		};
	}
}
