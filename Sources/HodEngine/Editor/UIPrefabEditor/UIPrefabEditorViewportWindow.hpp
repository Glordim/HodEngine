#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

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

		void		DrawGrid(const Vector2& viewCenter, float worldHalfWidth, float worldHalfHeight, float scale, RenderView& renderView);
		void		DrawNode(ui2::Node* node, RenderView& renderView);
		ui2::Node*	PickNode(ui2::Node* node, const Vector2& canvasPosition);

	private:

		RenderTarget* _renderTarget = nullptr;

		Vector2 _cameraPosition = Vector2::Zero;
		float   _zoom = 1.0f;
		float   _targetZoom = 1.0f;
	};
}
