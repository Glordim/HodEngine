#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <cstdint>

struct ImGuiMultiSelectIO;

namespace hod::inline game
{
	class Entity;
	class Component;
}

namespace hod::inline editor
{
	class HierachyWindow;

	/// @brief What a drag of an entity of the hierarchy carries: the dragged entity (the selection goes along when it is in it)
	struct EntityDragAndDropPayload
	{
		HierachyWindow*	_hierarchyWindow;
		Entity*			_entity;
	};

	/// @brief The entities of an EntityEditorTab's world as a tree: multi-selection (Ctrl/Shift click, box select, Ctrl+A), drag and drop to
	/// move them, rename on the label, context menu and shortcuts for the hierarchy's edits (see EntityEditorTab)
	class HOD_GAME_EDITOR_API HierachyWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(HierachyWindow, EditorTabWindow)

	public:

					HierachyWindow() = default; // todo remove
					HierachyWindow(EditorTab* editorTab);
					~HierachyWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawEntity(Entity* entity);
		void		DrawContextMenu();
		void		HandleShortcuts();
		void		ApplyPendingEdits();

		// The multi-selection's requests (see ImGui::BeginMultiSelect), whose user data is the entity. Ranges are resolved against
		// _visibleEntities, so only at the scope's end (ImGui only asks for them there); a select all at its beginning is deferred to the end
		void		ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, bool scopeEnd);

		Vector<Entity*>	GetDraggedEntities(Entity* entity) const;

		// Rename: the entity's label replaced by a text field, Enter or a click elsewhere validates, Escape cancels
		void		StartRename(Entity* entity);
		void		ApplyRename();

	private:

		bool						_openContextualMenu = false;

		EntityDragAndDropPayload	_entityDragAndDropPayload;

		// The entities drawn this frame, in the tree's order (for the multi-selection's ranges and select all)
		Vector<Entity*>				_visibleEntities;
		bool						_selectAllRequested = false;
		// Reveal (see DrawContent)
		WeakPtr<Entity>				_lastActiveEntity;
		bool						_revealSelection = false;

		WeakPtr<Entity>				_renamedEntity;
		char						_renameBuffer[256] = {};
		bool						_renameFocus = false;		// the field takes the keyboard focus on its first draw
		bool						_renameDrawn = false;		// the field was drawn this frame

		// Done once the tree is drawn, not while iterating over it (see ApplyPendingEdits). Delete, duplicate: the selection's.
		bool						_pendingDelete = false;
		bool						_pendingDuplicate = false;
		bool						_pendingPaste = false;
		bool						_pendingPasteInSelection = false; // as children of the selection, not next to it
		bool						_pendingReparent = false;
		bool						_pendingReparentToRoot = false; // to the scene's roots (no parent)
		Vector<WeakPtr<Entity>>		_pendingReparentEntities;
		WeakPtr<Entity>				_pendingReparentParent;
		uint32_t					_pendingReparentIndex = UINT32_MAX;
	};
}
