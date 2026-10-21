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
		void		StartRename(ui2::Node* node);
		void		ApplyRename();

	private:

		// Selection seen at the previous frame: a different one (made from here or from elsewhere, e.g.
		// the viewport) triggers a one-frame reveal, see DrawContent().
		ui2::Node*	_lastSelectedNode = nullptr;
		bool		_revealSelection = false;

		// Reparenting requested by a drag and drop, applied once the whole tree has been drawn: doing it
		// in place would modify a children list DrawNode() is iterating over.
		ui2::Node*	_pendingReparentNode = nullptr;
		ui2::Node*	_pendingReparentParent = nullptr;

		// "Convert To" requested from the context menu, applied after the tree has been drawn for the same reason.
		ui2::Node*						_pendingConvertNode = nullptr;
		const ReflectionDescriptor*		_pendingConvertDescriptor = nullptr;

		// Deletion requested (Delete key or context menu), applied after the tree has been drawn for the same reason.
		ui2::Node*	_pendingDeleteNode = nullptr;

		// Node whose label is replaced by a text field (F2 or "Rename"): Enter or focus lost validates, Escape cancels.
		ui2::Node*	_renamedNode = nullptr;
		char		_renameBuffer[256] = {'\0'};
		bool		_renameFocus = false;     // The field is to be focused on its next draw
		bool		_renameSubmitted = false; // The field was drawn this frame
	};
}
