#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Lightweight 2D preview of a UIPrefabEditorTab's ui2::Node tree: draws each node's
	/// computed rect (position/size) as a wireframe box via ImGui's draw list, with click-to-select.
	///
	/// Not a real rendered viewport (unlike PrefabEditorTab's, which uses the RHI-backed
	/// ViewportWindow/Gizmos/RenderView pipeline): UI2 has no rendering integration yet (no
	/// Drawable/Text/Image node types, see Docs/Architecture/UI2-Architecture.md), so there is
	/// nothing to render beyond node bounds regardless of which rendering technology is used.
	/// Known simplifications: rotation is ignored (boxes are drawn axis-aligned), and a node's box
	/// is centered on its computed canvas-space position, which is only exact for the default
	/// (centered) Origin.
	class HOD_EDITOR_API UIPrefabEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorViewportWindow, EditorTabWindow)

	public:

					UIPrefabEditorViewportWindow() = default;
					UIPrefabEditorViewportWindow(EditorTab* editorTab);
					~UIPrefabEditorViewportWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawNode(ui2::Node* node, const ImVec2& origin, float scale);
	};
}
