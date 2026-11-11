#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <memory>

namespace hod::inline core
{
	class UID;
}

namespace hod::inline game
{
	class World;
	class Scene;
	class Entity;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API EntityEditorTab : public EditorTab
	{
	public:
				EntityEditorTab(std::shared_ptr<Asset> asset, const char* icon = nullptr);
				~EntityEditorTab() override;

		void	Play();
		void	Stop();
		void	Pause();
		void	Resume();
		void	PlayNextFrame();

		bool	IsPlaying() const;
		bool	IsPaused() const;

		void	ReloadScene();

		game::World*	GetWorld() const;
		Scene*	GetCurrentScene() const;

		// Selection: any number of entities, the last one selected being the active one (the one the inspector shows, F2 renames, the
		// gizmo moves...). Entities of the edited world, or of the playing one's copy while playing.
		// The active entity, nullptr if none
		Entity*			GetEntitySelection() const;
		// In the order they were selected, the active one last (the ones deleted since left out)
		Vector<Entity*>	GetSelectedEntities() const;
		bool			IsSelected(const Entity* entity) const;
		// entity only, nothing if nullptr
		void			SetEntitySelection(Entity* entity);
		// entities only, the last one active
		void			SetSelectedEntities(const Vector<Entity*>& entities);
		// Adds entity, as the active one, or removes it
		void			SetEntitySelected(Entity* entity, bool selected);
		// The selected entities but those inside another selected one, in the hierarchy's order: what acts on an entity along with its
		// descendants (copy, delete, move...) takes them
		Vector<Entity*>	GetSelectedSubtrees() const;

		// The entity all the other ones are under (a prefab's root, see PrefabEditorTab), nullptr if there can be any number of roots (a scene)
		virtual Entity*	GetRootEntity() const;
		// Whether entity is the root entity: it can't be moved, deleted nor renamed
		bool			IsRootEntity(const Entity* entity) const;
		// Where an entity is created when nothing else gives its parent: under the root entity if any
		Entity*			GetDefaultParent() const;
		// Whether entity can be given parent (nullptr: made a root): not if it is the root entity, nor into itself or its descendants,
		// nor out of the root entity
		bool			CanReparent(const Entity* entity, const Entity* parent) const;
		// Whether an instance of the prefab can be added (not into itself)
		virtual bool	CanInstantiatePrefab(const UID& prefab) const;

		// Edits of the hierarchy, marked as edits. Not the root entity (see IsRootEntity): skipped.
		// Destroys entities (but those inside another one of them) and their descendants, deselected
		void			DeleteEntities(const Vector<Entity*>& entities);
		// A copy of each entity (but those inside another one of them) and its descendants, right after it under the same parent, the
		// copies selected
		// @return the copies
		Vector<Entity*>	DuplicateEntities(const Vector<Entity*>& entities);
		// Whether ReparentEntities would move something: they can all be given parent (see CanReparent) and one of them at least isn't
		// already its child
		bool			CanReparentEntities(const Vector<Entity*>& entities, const Entity* parent) const;
		// Moves entities (but those inside another one of them), in the hierarchy's order, as children of parent (null: roots of their
		// scene) from index among the siblings that stay (the last places if out of range), selected. Refused if CanReparentEntities is false.
		bool			ReparentEntities(const Vector<Entity*>& entities, Entity* parent, uint32_t index = UINT32_MAX);

		// Clipboard: entities and their descendants (one inside another of them comes with it) are copied as text (a JSON document
		// of SceneSerializer's entities) to the system clipboard, so that they can be pasted in another scene or prefab too.
		// Cancels a pending cut.
		bool			CopyEntities(const Vector<Entity*>& entities);
		// As in a file explorer: entities are copied (see CopyEntities) and marked as cut, but stay in place until pasted in this
		// tab, which moves them. Cancelled by CancelCut (Escape), another copy, or the clipboard changing.
		// @return false if one of them can't be moved (the root entity): nothing is cut then
		bool			CutEntities(const Vector<Entity*>& entities);
		void			CancelCut();
		// Whether entity, or one of its ancestors, is marked as cut
		bool			IsCut(const Entity* entity) const;
		bool			HasCutEntities() const;
		// Whether the clipboard holds copied entities
		bool			CanPaste() const;
		// Pastes the clipboard's entities, as children of parent (nullptr: roots of the scene, not in a prefab) from index among its
		// children (the last place if out of range), with new local ids, the last one selected. The entities cut in this tab are moved
		// there instead, unless parent is one of them or inside one. Not an instance of a prefab into itself (see CanInstantiatePrefab).
		// To call after the drawing of the entities, not while iterating over them.
		// @return the pasted (or moved) roots, empty if nothing was pasted
		Vector<Entity*>	PasteEntities(Entity* parent, uint32_t index = UINT32_MAX);
		// Right after the selected entity among its siblings (to chain pastes), or as the last children of the default parent (see
		// GetDefaultParent) when the selection is the root entity or none
		Vector<Entity*>	PasteNextToSelection();
		// As the last children of the selected entity, or of the default parent when there is no selection
		Vector<Entity*>	PasteInSelection();
		// Ctrl+C / Ctrl+X / Ctrl+V (next to the selection) / Ctrl+Shift+V (in the selection) on the selection, Escape cancelling a cut,
		// for the focused window calling it. Same contract as PasteEntities.
		void			HandleClipboardShortcuts();

		// F, pressed in the hierarchy or the viewport: the viewport centers its view on the selection
		void			RequestFrameSelection();
		// @return true if a framing was requested since the last call
		bool			ConsumeFrameSelectionRequest();

	protected:
	
		void DrawMenuBar() override;

		// Undo: the edited scene as saved (see Scene::SerializeInDocument). Restoring it rebuilds all its entities: the windows keeping
		// entities across frames hold WeakPtrs. The selection follows, found back by its local ids from its root.
		// Not while playing: the edits then go to the playing world's copy.
		bool CaptureUndoState(Document& document) override;
		bool RestoreUndoState(const Document& document) override;
		bool IsUndoAvailable() const override;
		
	protected:

		game::World*				_world = nullptr;

		game::World*				_editingWorld = nullptr;
		game::World*				_playingWorld = nullptr;

		Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;

		Vector<WeakPtr<Entity>>	_selection; // the active one last
		bool				_frameSelectionRequested = false;

		// Cut (see CutEntities): the cut entities, and the clipboard's text the cut put there, which must still be there for the paste
		// to move them
		Vector<WeakPtr<Entity>>	_cutEntities;
		String					_cutClipboardText;
	};
}
