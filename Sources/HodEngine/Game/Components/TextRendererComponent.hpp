#pragma once

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

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
		class TextRendererComponent : public RendererComponent
		{
			REFLECTED_CLASS(TextRendererComponent, RendererComponent)

		public:

											TextRendererComponent();
											TextRendererComponent(const TextRendererComponent&) = delete;
											TextRendererComponent(TextRendererComponent&&) = delete;
											~TextRendererComponent() override;

			TextRendererComponent&			operator=(const TextRendererComponent&) = delete;
			TextRendererComponent&			operator=(TextRendererComponent&&) = delete;

		public:
		
			void							OnConstruct() override;

			renderer::MaterialInstance*		GetMaterialInstance() const;
			void							SetMaterialInstance(renderer::MaterialInstance* materialInstance);

			void							SetFont(const WeakResource<renderer::FontResource>& font);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;
			void							PushPickingToRenderQueue(renderer::RenderQueue& renderQueue, const Color& colorId) override;

		private:

			WeakResource<renderer::FontResource>	_fontResource;
			
			renderer::MaterialInstance*				_materialInstance = nullptr;
		};
	}
}
