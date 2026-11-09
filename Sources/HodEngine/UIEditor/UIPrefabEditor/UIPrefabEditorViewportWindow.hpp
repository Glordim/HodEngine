#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorViewportSettings.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI/LayoutParams.hpp>

namespace hod::inline ui
{
	class Node;
	class Canvas;
}

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline editor
{
	/// @brief RT-rendered 2D preview of a UIPrefabEditorTab's ui::Node tree: draws the canvas' content as in game
	/// (ui::Canvas::PushRenderCommand) into an offscreen render target, then, over it and via ShapeDrawer, each node's
	/// computed rect as an outline (the selected ones in orange), the active node's spacing and the manipulation gizmo (the selected
	/// node's, or with several selected the box around them, moving them together, see GetGizmoNodes), and blits that texture into the panel. Click-to-select is done in canvas space
	/// against the same node bounds, Ctrl+click (or Shift+click) adding to the selection or removing from it.
	///
	/// The prefab can be previewed on a device picked from a user-editable list (name + resolution, optionally
	/// rotated): the canvas is then laid out for that resolution, exactly as it would be in game (see
	/// ui::CanvasScaler), so anchors and layouts can be checked on other aspect ratios. Switching device eases the
	/// resolution towards the new one, so the layout morphs progressively. The device's frame is outlined and labeled.
	/// Never saved in the prefab: the devices, the active one and the grid settings are persisted per user.
	/// In play mode (see UIPrefabEditorTab::Play): the mouse then drives
	/// the prefab's Interactions instead of selecting and moving nodes, with what the pointer reaches outlined (see DrawInteractionOverlay).
	/// Known simplification: rotation and scale are ignored by the editor overlays (outlines, picking and gizmo are
	/// axis-aligned, at the layout size, centered on the node's computed canvas-space center).
	class HOD_UI_EDITOR_API UIPrefabEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorViewportWindow, EditorTabWindow)

	public:

					UIPrefabEditorViewportWindow() = default;
					UIPrefabEditorViewportWindow(EditorTab* editorTab);
					~UIPrefabEditorViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		using Device = UIPrefabEditorViewportSettings::Device;

		void		DrawToolbar();
		void		DrawViewport(bool mouseOverToolbar);
		void		DrawDevicesPopup();
		void		DrawGridSettingsPopup();
		void		DrawPreviewFrames(RenderView& renderView);
		void		DrawPreviewLabels(const ImVec2& imagePos, const ImVec2& imageSize, float scale);
		Vector2		GetDeviceFrameSize() const;
		void		DrawGrid(const Vector2& origin, const Vector2& viewCenter, float worldHalfWidth, float worldHalfHeight, float scale, RenderView& renderView);
		void		DrawNode(ui::Node* node, RenderView& renderView);
		void		DrawSpacing(ui::Node* node, RenderView& renderView);
		ui::Node*	PickNode(ui::Node* node, const Vector2& canvasPosition);

		ui::Node*	UpdateInteraction(ui::Canvas& canvas, const Vector2& canvasMousePosition, bool mouseAvailable);
		void		DrawInteractionOverlay(ui::Canvas& canvas, ui::Node* hitNode, RenderView& renderView);

		// Manipulation gizmo drawn over the active node (move via the node's body, resize via the 4 corners / 4 edges),
		// hit-tested on the CPU against canvas-space bounds rather than with a picking texture,
		// since this viewport isn't a ViewportWindow and has no picking render target.
		enum class GizmoHandle : uint8_t
		{
			None,
			Move,
			Top,
			Bottom,
			Left,
			Right,
			TopLeft,
			TopRight,
			BottomLeft,
			BottomRight,
		};

		// What the gizmo may do to a node: its LayoutParams say what its parent leaves to it
		struct GizmoAxes
		{
			bool _move = false;
			bool _resizeX = false;
			bool _resizeY = false;

			bool Any() const { return _move || _resizeX || _resizeY; }
		};

		static GizmoAxes GetGizmoAxes(const ui::Node& node);

		// The nodes the gizmo acts on: the selected node, moved and resized; with several selected, the ones it can move (the
		// subtrees' roots, a child moving with its parent), moved together and never resized.
		Vector<ui::Node*> GetGizmoNodes() const;
		static void        ComputeCanvasBounds(const Vector<ui::Node*>& nodes, Vector2& center, Vector2& size);

		void		DrawSelectionGizmo(const Vector<ui::Node*>& nodes, float scale, RenderView& renderView);
		GizmoHandle	HitTestGizmoHandle(const Vector<ui::Node*>& nodes, const Vector2& canvasMousePosition, float scale) const;
		void		UpdateGizmoDrag(const Vector<ui::Node*>& nodes, const Vector2& canvasMousePosition, float scale, bool mouseAvailable);

	private:

		RenderTarget* _renderTarget = nullptr;

		// Devices/grid settings, loaded from and saved to the user settings (see UIPrefabEditorViewportSettings).
		UIPrefabEditorViewportSettings _settings;
		bool                           _settingsDirty = false;

		// Entering the play mode turns the preview on (still toggleable meanwhile), leaving it gives back the setting it had before.
		bool _wasPlaying = false;
		bool _previewBeforePlay = false;

		// Resolution the canvas is currently laid out for, easing towards the previewed device's one. Zero until the first frame (no easing then).
		Vector2 _previewResolution = Vector2::Zero;

		// Pixels per canvas unit fitting the previewed device's frame in the viewport at zoom 1, easing towards the
		// final device's fit along with _previewResolution. Zero until the first frame.
		float _baseScale = 0.0f;

		// Zoom relative to _baseScale: the view draws at _baseScale * _camera.GetZoom() pixels per canvas unit.
		FramingCamera _camera;
		bool          _frameDeviceRequested = false; // a device was picked in the list: its frame is framed on the next frame

		// Play mode: whether the left button is down after a press started over the viewport (see UpdateInteraction)
		bool _interactionPointerDown = false;

		GizmoHandle _hoveredGizmoHandle = GizmoHandle::None;
		GizmoHandle _draggedGizmoHandle = GizmoHandle::None;
		Vector2     _gizmoDragStartMousePosition = Vector2::Zero;

		// A node dragged, as it was when the drag started
		struct GizmoDragStart
		{
			ui::Node* _node = nullptr;
			// Its rect (center in its parent's space), which the drag moves/resizes
			Vector2    _center = Vector2::Zero;
			Vector2    _size = Vector2::Zero;
			// What placed it, restored by Escape (a drag may turn a Content axis into a Fixed one, see AnchoredLayoutParams::PlaceRect).
			// The offset is AnchoredLayoutParams' only
			Vector2                     _offset = Vector2::Zero;
			Vector2                     _ownSize = Vector2::Zero;
			ui::LayoutParams::SizeMode _widthMode = ui::LayoutParams::SizeMode::Fixed;
			ui::LayoutParams::SizeMode _heightMode = ui::LayoutParams::SizeMode::Fixed;
		};
		Vector<GizmoDragStart> _gizmoDragStarts;
	};
}
