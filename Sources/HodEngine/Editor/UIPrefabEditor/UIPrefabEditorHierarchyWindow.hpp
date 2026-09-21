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
	/// add/remove-child editing, drop target for nodes dragged from the Library, and reparenting by
	/// dragging a node onto its new parent (added last among its children). The ui2::Node equivalent
	/// of HierachyWindow, but self-contained (no World/Entity involved).
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

	private:

		// Selection seen at the previous frame: a different one (made from here or from elsewhere, e.g.
		// the viewport) triggers a one-frame reveal, see DrawContent().
		ui2::Node*	_lastSelectedNode = nullptr;
		bool		_revealSelection = false;

		// Reparenting requested by a drag and drop, applied once the whole tree has been drawn: doing it
		// in place would modify a children list DrawNode() is iterating over.
		ui2::Node*	_pendingReparentNode = nullptr;
		ui2::Node*	_pendingReparentParent = nullptr;
	};
}
