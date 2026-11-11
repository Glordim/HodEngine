#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Vector.hpp>

struct ImGuiMultiSelectIO;

namespace hod::inline ui
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Tree view of a UIPrefabEditorTab's ui::Node hierarchy, with multi-selection (as in a file explorer: Ctrl+click,
	/// Shift+click, box selection, Ctrl+A, see UIPrefabEditorTab::GetSelectedNodes) and basic add/remove-child editing, drop
	/// target for nodes dragged from the Library, and reparenting by dragging nodes (the selection when the dragged node is in
	/// it) onto their new parent (added last among its children), copy / cut / paste / duplicate / delete of the selection (see
	/// UIPrefabEditorTab::CopyNodes), and nested prefabs: a UI prefab dropped from the asset browser is instantiated, the instances
	/// shown in blue with their own actions (see UIPrefabEditorTab::NodeRole). The ui::Node equivalent
	/// of HierachyWindow, but self-contained (no World/Entity involved).
	class HOD_UI_EDITOR_API UIPrefabEditorHierarchyWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorHierarchyWindow, EditorTabWindow)

	public:

					UIPrefabEditorHierarchyWindow() = default;
					UIPrefabEditorHierarchyWindow(EditorTab* editorTab);
					~UIPrefabEditorHierarchyWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawNode(ui::Node* node);
		void		DrawVoidContextMenu();
		void		StartRename(ui::Node* node);
		void		ApplyRename();
		// The multi-selection's requests (see ImGui::BeginMultiSelect), whose user data is the node. Ranges are resolved against
		// _visibleNodes, so only at the scope's end (ImGui only asks for them there); a select all at its beginning is deferred to
		// the end for the same reason (see _selectAllRequested).
		void		ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, bool scopeEnd);

	private:

		// Active node seen at the previous frame: a different one made from elsewhere (e.g. the viewport) triggers a one-frame
		// reveal, see DrawContent().
		ui::Node*	_lastSelectedNode = nullptr;
		bool		_revealSelection = false;

		// The nodes drawn this frame, in order (those under collapsed parents aren't): what a range selection spans
		Vector<ui::Node*>	_visibleNodes;
		bool				_selectAllRequested = false;

		// The tab's UIPrefabEditorTab::GetRebuildCount() seen at the previous frame: the node pointers kept across frames are dropped on a change
		uint32_t	_rebuildCount = 0;

		// Reparenting requested by a drag and drop, applied once the whole tree has been drawn: doing it
		// in place would modify a children list DrawNode() is iterating over.
		Vector<ui::Node*>	_pendingReparentNodes;
		ui::Node*			_pendingReparentParent = nullptr;
		ui::Node*			_pendingReparentBefore = nullptr; // null: as the last children

		// "Convert To" requested from the context menu, applied after the tree has been drawn for the same reason.
		ui::Node*						_pendingConvertNode = nullptr;
		const ReflectionDescriptor*		_pendingConvertDescriptor = nullptr;

		// Deletion requested (Delete key or context menu), applied after the tree has been drawn for the same reason.
		Vector<ui::Node*>	_pendingDeleteNodes;

		// Paste / duplicate requested from the context menu, applied after the tree has been drawn for the same reason. Pasted
		// right after the active node, or, with _pendingPasteParent, as that node's last children.
		bool				_pendingPaste = false;
		ui::Node*			_pendingPasteParent = nullptr;
		Vector<ui::Node*>	_pendingDuplicateNodes;

		// Node whose label is replaced by a text field (F2 or "Rename"): Enter or focus lost validates, Escape cancels.
		ui::Node*	_renamedNode = nullptr;
		char		_renameBuffer[256] = {'\0'};
		bool		_renameFocus = false;     // The field is to be focused on its next draw
		bool		_renameSubmitted = false; // The field was drawn this frame
	};
}
