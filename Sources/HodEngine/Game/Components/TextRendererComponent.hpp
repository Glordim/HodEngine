#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include <string>

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
		class HOD_API TextRendererComponent : public RendererComponent
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
			//void							SetText(const std::string_view& text);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue) override;
			void							PushPickingToRenderQueue(renderer::RenderQueue& renderQueue, const Color& colorId) override;

		private:

			std::string								_text = "EditMe";

			WeakResource<renderer::FontResource>	_fontResource;
			
			renderer::MaterialInstance*				_materialInstance = nullptr;
		};
	}
}
