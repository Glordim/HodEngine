#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

namespace hod::inline editor
{
	/// @brief Draws the properties of a UIPrefabEditorTab's selected ui2::Node, plus its
	/// LayoutParams' own properties when it has one, via the engine's generic reflected-property
	/// drawer (no custom per-type UI needed for now).
	class HOD_EDITOR_API UIPrefabEditorInspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorInspectorWindow, EditorTabWindow)

	public:

					UIPrefabEditorInspectorWindow() = default;
					UIPrefabEditorInspectorWindow(EditorTab* editorTab);
					~UIPrefabEditorInspectorWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
