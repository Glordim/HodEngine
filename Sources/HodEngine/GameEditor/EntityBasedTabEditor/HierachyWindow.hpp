#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <memory>

namespace hod::inline game
{
	class Entity;
	class Component;
}

namespace hod::inline editor
{
	class HierachyWindow;

	/// @brief 
	struct EntityDragAndDropPayload
	{
		HierachyWindow*	_hierarchyWindow;
		Entity*	_entity;
	};

	/// @brief 
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

		// Rename: the entity's label replaced by a text field, Enter or a click elsewhere validates, Escape cancels
		void		StartRename(Entity* entity);
		void		ApplyRename();

	private:

		bool						_openContextualMenu = false;

		EntityDragAndDropPayload	_entityDragAndDropPayload;

		Entity*						_previousSelection = nullptr;

		WeakPtr<Entity>				_renamedEntity;
		char						_renameBuffer[256] = {};
		bool						_renameFocus = false;		// the field takes the keyboard focus on its first draw
		bool						_renameDrawn = false;		// the field was drawn this frame

		// Done once the tree is drawn, not while iterating over it
		WeakPtr<Entity>				_pendingDelete;
		WeakPtr<Entity>				_pendingDuplicate;
		bool						_pendingPaste = false;
		bool						_pendingPasteInSelection = false; // as children of the selection, not next to it
	};
}
