#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"

namespace hod::inline ui2
{
	class Canvas;
}

namespace hod::inline game
{
	/// @brief 
	class HOD_GAME_API UIComponent : public RendererComponent
	{
		REFLECTED_CLASS(UIComponent, RendererComponent)

	public:

										UIComponent();
										UIComponent(const UIComponent&) = delete;
										UIComponent(UIComponent&&) = delete;
										~UIComponent() override;

		UIComponent&					operator=(const UIComponent&) = delete;
		UIComponent&					operator=(UIComponent&&) = delete;

		void	OnConstruct() override;
		void	OnDestruct() override;

		// In game (playing, not paused) the canvas is drawn on the UI queue, scaled to fit the screen;
		// otherwise (editor scene view) it is drawn in the world, 1 canvas unit = 0.01 world unit, like
		// the previous UI system did.
		void	PushRenderCommand(RenderView& renderView) override;
		Rect	GetBoundingBox() const override;

	private:

		//WeakResource<UIPrefabResource>	_canvas;
		ui2::Canvas* _canvas;
	};
}
