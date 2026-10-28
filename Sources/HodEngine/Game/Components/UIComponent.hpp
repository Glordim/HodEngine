#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"

#include <HodEngine/UI2/Canvas.hpp>

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

		// In game (playing, not paused) the canvas is drawn on the UI queue, laid out and scaled for the screen per its effective scaler;
		// otherwise (editor scene view) it is drawn in the world, 1 canvas unit = 0.01 world unit, like
		// the previous UI system did.
		void	PushRenderCommand(RenderView& renderView) override;
		Rect	GetBoundingBox() const override;

		// How the canvas adapts to the screen (design resolution, scale mode), applied to the canvas.
		// Only in effect when overriding the project's (ui2::UISettings), see ui2::Canvas::SetOverrideScaler.
		const ui2::CanvasScaler&	GetScaler() const;
		void						SetScaler(const ui2::CanvasScaler& scaler);

		bool						GetOverrideScaler() const;
		void						SetOverrideScaler(bool overrideScaler);

	private:

		//WeakResource<UIPrefabResource>	_canvas;
		ui2::Canvas* _canvas = nullptr;

		bool				_overrideScaler = false;
		ui2::CanvasScaler	_scaler;
	};
}
