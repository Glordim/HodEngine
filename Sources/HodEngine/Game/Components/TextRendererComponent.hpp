#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/String.hpp"

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
		class HOD_GAME_API TextRendererComponent : public RendererComponent
		{
			REFLECTED_CLASS(TextRendererComponent, RendererComponent)

		public:
			TextRendererComponent();
			TextRendererComponent(const TextRendererComponent&) = delete;
			TextRendererComponent(TextRendererComponent&&) = delete;
			~TextRendererComponent() override;

			TextRendererComponent& operator=(const TextRendererComponent&) = delete;
			TextRendererComponent& operator=(TextRendererComponent&&) = delete;

		public:
			void OnConstruct() override;

			renderer::MaterialInstance* GetMaterialInstance() const;
			void                        SetMaterialInstance(renderer::MaterialInstance* materialInstance);

			void SetFont(const WeakResource<renderer::FontResource>& font);
			// void							SetText(const std::string_view& text);

			void PushRenderCommand(renderer::RenderView& renderView) override;
			Rect GetBoundingBox() const override;

		private:
			String _text = "EditMe";

			WeakResource<renderer::FontResource> _fontResource;

			renderer::MaterialInstance* _materialInstance = nullptr;
		};
	}
}
