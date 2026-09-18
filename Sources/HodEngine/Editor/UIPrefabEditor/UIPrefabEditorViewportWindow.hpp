#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportSettings.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <vector>

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline renderer
{
	class RenderTarget;
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
	/// A user-editable list of guides (name + resolution + color, individually toggleable) can be
	/// overlaid: each is a rect anchored on the canvas' top-left corner (shared by all guides), labeled at
	/// its bottom-right corner, to check the layout against target resolutions/safe areas. The guides and grid settings are persisted per user.
	/// Known simplifications: rotation is ignored (boxes are drawn axis-aligned), and a node's box
	/// is centered on its computed canvas-space position, which is only exact for the default
	/// (centered) Origin.
	class HOD_EDITOR_API UIPrefabEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorViewportWindow, EditorTabWindow)

	public:

					UIPrefabEditorViewportWindow() = default;
					UIPrefabEditorViewportWindow(EditorTab* editorTab);
					~UIPrefabEditorViewportWindow() override;

	public:

		void		DrawContent() override;

	private:

		using Guide = UIPrefabEditorViewportSettings::Guide;

		std::vector<const Guide*>	GetGuidesToDraw() const;
		void		DrawToolbar();
		void		DrawGuidesSettingsPopup();
		void		DrawGridSettingsPopup();
		void		DrawGuides(const Vector2& origin, RenderView& renderView);
		void		DrawGuideLabels(const Vector2& origin, const ImVec2& imagePos, const ImVec2& imageSize, float scale);
		void		DrawGrid(const Vector2& origin, const Vector2& viewCenter, float worldHalfWidth, float worldHalfHeight, float scale, RenderView& renderView);
		void		DrawNode(ui2::Node* node, RenderView& renderView);
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

		void		DrawSelectionGizmo(ui2::Node* node, float scale, RenderView& renderView);
		GizmoHandle	HitTestGizmoHandle(ui2::Node* node, const Vector2& canvasMousePosition, float scale) const;
		void		UpdateGizmoDrag(ui2::Node* node, const Vector2& canvasMousePosition, float scale, bool mouseAvailable);

	private:

		RenderTarget* _renderTarget = nullptr;

		// Guides/grid settings, loaded from and saved to the user settings (see UIPrefabEditorViewportSettings).
		UIPrefabEditorViewportSettings _settings;
		bool                           _settingsDirty = false;

		Vector2 _cameraPosition = Vector2::Zero;
		float   _zoom = 1.0f;
		float   _targetZoom = 1.0f;

		GizmoHandle _hoveredGizmoHandle = GizmoHandle::None;
		GizmoHandle _draggedGizmoHandle = GizmoHandle::None;
		Vector2     _gizmoDragStartMousePosition = Vector2::Zero;
		Vector2     _gizmoDragStartOffset = Vector2::Zero;
		Vector2     _gizmoDragStartDesiredSize = Vector2::Zero;
	};
}
