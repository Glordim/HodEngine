#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorViewportSettings.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI2/LayoutParams.hpp>

namespace hod::inline ui2
{
	class Node;
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
	/// @brief RT-rendered 2D preview of a UIPrefabEditorTab's ui2::Node tree: draws each node's
	/// computed rect (position/size) as a wireframe box into an offscreen render target (via
	/// Gizmos/RenderView, like the other RHI-backed editor viewports), then blits that texture
	/// into the panel. Click-to-select is done in canvas space against the same node bounds.
	///
	/// ui2::Node has no drawable content yet (no Drawable/Text/Image node types, see
	/// Docs/Architecture/UI2-Architecture.md), so there is nothing to render beyond node bounds
	/// regardless of which rendering technology is used.
	///
	/// The prefab can be previewed on a device picked from a user-editable list (name + resolution, optionally
	/// rotated): the canvas is then laid out for that resolution, exactly as it would be in game (see
	/// ui2::CanvasScaler), so anchors and layouts can be checked on other aspect ratios. Switching device eases the
	/// resolution towards the new one, so the layout morphs progressively. The device's frame is outlined and labeled.
	/// Never saved in the prefab: the devices, the active one and the grid settings are persisted per user.
	/// Known simplification: rotation and scale are ignored (boxes are drawn axis-aligned, at the layout size,
	/// centered on the node's computed canvas-space center).
	class HOD_UI2_EDITOR_API UIPrefabEditorViewportWindow : public EditorTabWindow
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
		void		DrawNode(ui2::Node* node, RenderView& renderView);
		void		DrawSpacing(ui2::Node* node, RenderView& renderView);
		ui2::Node*	PickNode(ui2::Node* node, const Vector2& canvasPosition);

		// Manipulation gizmo drawn over the selected node, mirroring UIEditor's
		// NodeCustomComponentDrawer (move via the node's body, resize via the 4 corners / 4 edges),
		// but hit-tested on the CPU against canvas-space bounds instead of the RHI picking texture,
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

		static GizmoAxes GetGizmoAxes(const ui2::Node& node);

		void		DrawSelectionGizmo(ui2::Node* node, float scale, RenderView& renderView);
		GizmoHandle	HitTestGizmoHandle(ui2::Node* node, const Vector2& canvasMousePosition, float scale) const;
		void		UpdateGizmoDrag(ui2::Node* node, const Vector2& canvasMousePosition, float scale, bool mouseAvailable);

	private:

		RenderTarget* _renderTarget = nullptr;

		// Devices/grid settings, loaded from and saved to the user settings (see UIPrefabEditorViewportSettings).
		UIPrefabEditorViewportSettings _settings;
		bool                           _settingsDirty = false;

		// Resolution the canvas is currently laid out for, easing towards the previewed device's one. Zero until the first frame (no easing then).
		Vector2 _previewResolution = Vector2::Zero;

		// Pixels per canvas unit fitting the previewed device's frame in the viewport at zoom 1, easing towards the
		// final device's fit along with _previewResolution. Zero until the first frame.
		float _baseScale = 0.0f;

		// Zoom relative to _baseScale: the view draws at _baseScale * _camera.GetZoom() pixels per canvas unit.
		FramingCamera _camera;
		bool          _frameDeviceRequested = false; // a device was picked in the list: its frame is framed on the next frame

		GizmoHandle _hoveredGizmoHandle = GizmoHandle::None;
		GizmoHandle _draggedGizmoHandle = GizmoHandle::None;
		Vector2     _gizmoDragStartMousePosition = Vector2::Zero;
		// The node's rect when the drag started (center in its parent's space), which the drag moves/resizes
		Vector2     _gizmoDragStartCenter = Vector2::Zero;
		Vector2     _gizmoDragStartSize = Vector2::Zero;
		// What placed it then, restored by Escape (a drag may turn a Content axis into a Fixed one, see AnchoredLayoutParams::PlaceRect).
		// The offset is AnchoredLayoutParams' only
		Vector2                     _gizmoDragStartOffset = Vector2::Zero;
		Vector2                     _gizmoDragStartOwnSize = Vector2::Zero;
		ui2::LayoutParams::SizeMode _gizmoDragStartWidthMode = ui2::LayoutParams::SizeMode::Fixed;
		ui2::LayoutParams::SizeMode _gizmoDragStartHeightMode = ui2::LayoutParams::SizeMode::Fixed;
	};
}
