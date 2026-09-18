#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Tree view of a UIPrefabEditorTab's ui2::Node hierarchy, with selection and basic
	/// add/remove-child editing, and drop target for nodes dragged from the Library. The ui2::Node
	/// equivalent of HierachyWindow, but self-contained (no World/Entity involved, no drag-drop
	/// reparenting yet).
	class HOD_EDITOR_API UIPrefabEditorHierarchyWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorHierarchyWindow, EditorTabWindow)

	public:

					UIPrefabEditorHierarchyWindow() = default;
					UIPrefabEditorHierarchyWindow(EditorTab* editorTab);
					~UIPrefabEditorHierarchyWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawNode(ui2::Node* node);
	};
}
