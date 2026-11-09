#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline renderer
{
	class ReadbackRenderTarget;
	class RenderView;
}

namespace hod::inline game
{
	class Entity;
}

namespace hod::inline editor
{
	/// @brief
	class HOD_GAME_EDITOR_API ViewportWindow : public EditorTabWindow, public GizmoViewport
	{
		REFLECTED_CLASS(ViewportWindow, EditorTabWindow)

	public:
		ViewportWindow() = default; // todo remove
		ViewportWindow(EditorTab* editorTab);
		~ViewportWindow() override;

	public:
		bool Draw() override;
		void DrawContent() override;

		RenderView*   GetRenderView() override;
		ReadbackRenderTarget* GetPickingRenderTarget() const override;
		const Matrix4&          GetProjectionMatrix() const override;
		const Matrix4&          GetViewMatrix() const override;
		float                   GetCameraSize() const override;

		const Vector2& GetPlayRatio() const override;

		/// @brief true while the mouse is used by the toolbar floating over the viewport, the viewport and its gizmos must then ignore it
		bool IsMouseOverToolbar() const override;

	private:
		void DrawToolbar();
		void DrawViewport();

		/// @brief The world's render, or the enabled debug view's (see ViewportDebugView)
		void DrawWorld(RenderView& renderView);

		void MoveSelectionAlong(Entity* activeEntity, const Vector2& worldMove);

		Vector2 ScreenToWorld(const ImVec2& screenPosition, const ImVec2& imagePosition, const ImVec2& imageSize) const;
		ImVec2  WorldToScreen(const Vector2& worldPosition, const ImVec2& imagePosition, const ImVec2& imageSize) const;

		// Rectangle selection: a left press not on a gizmo's handle, dragged past ImGui's drag threshold, selects the entities drawn inside
		// the rectangle instead of the clicked one (Ctrl or Shift held at the press: added to the selection). Escape gives back the selection
		// from before the press.
		enum class MarqueeState : uint8_t
		{
			None,
			Pending,   // pressed, not dragged far enough yet: still a click
			Active,    // dragging, the selection follows the rectangle
			Cancelled, // Escape: nothing more until the button is released
		};

		void            StartMarquee(const Vector2& worldPosition, bool additive);
		void            UpdateMarquee(const ImVec2& imagePosition, const ImVec2& imageSize);
		void            DrawMarquee(const ImVec2& imagePosition, const ImVec2& imageSize);
		Vector<Entity*> CollectEntitiesInRect(const Vector2& min, const Vector2& max) const;

		RenderView*   _renderView = nullptr;
		RenderTarget* _renderTarget = nullptr;
		ReadbackRenderTarget* _pickingRenderTarget = nullptr;

		Matrix4 _projection;
		Matrix4 _view;

		Vector2 _playRatio = Vector2(16.0f, 9.0f);

		// The camera, panned and zoomed as the UI prefab editor's (eased zoom, pan following the mouse at any zoom). At zoom 1 the view shows
		// DefaultCameraSize world units above and below its center (GetCameraSize: at the current zoom)
		static constexpr float DefaultCameraSize = 5.0f;

		FramingCamera _camera;

		bool _debugPicker = false;

		ViewportDebugViewMode _debugViewMode = ViewportDebugViewMode::Replace; // Kept when switching debug views

		bool  _drawGrid = true;
		float _gridCellSize = 1.0f; // world units, at the default camera size (see ViewportGrid)

		bool _mouseOverToolbar = false;

		MarqueeState            _marqueeState = MarqueeState::None;
		Vector2                 _marqueeStart = Vector2::Zero; // world space
		bool                    _marqueeAdditive = false;
		Vector<WeakPtr<Entity>> _marqueeSelectionBefore; // at the press: given back by Escape, kept by an additive rectangle
		ImVec4                  _marqueeLastRect;        // in image pixels, the one the selection was last computed for
		bool _gamePointerDown = false; // Playing: a press started over the image, the game keeps the mouse until released
	};
}
