#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Draws a UIPrefabEditorTab's selected ui2::Node as three blocks, laid out like the scene
	/// inspector's components: the Node's own properties, its Layout (how it arranges its children —
	/// add / transform / remove) and its LayoutParams (how its parent places it), all via the engine's
	/// generic reflected-property drawer (no custom per-type UI needed for now).
	class HOD_EDITOR_API UIPrefabEditorInspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorInspectorWindow, EditorTabWindow)

	public:

					UIPrefabEditorInspectorWindow() = default;
					UIPrefabEditorInspectorWindow(EditorTab* editorTab);
					~UIPrefabEditorInspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		bool		DrawLayoutBlock(ui2::Node* node);
	};
}
