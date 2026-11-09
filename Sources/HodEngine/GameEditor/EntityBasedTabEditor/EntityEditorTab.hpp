#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

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

		void			SetEntitySelection(Entity* selection);
		Entity*	GetEntitySelection() const;

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

		// F, pressed in the hierarchy or the viewport: the viewport centers its view on the selection
		void			RequestFrameSelection();
		// @return true if a framing was requested since the last call
		bool			ConsumeFrameSelectionRequest();

	protected:
	
		void DrawMenuBar() override;
		
	protected:

		game::World*				_world = nullptr;

		game::World*				_editingWorld = nullptr;
		game::World*				_playingWorld = nullptr;

		Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;

		WeakPtr<Entity>		_entitySelection;
		bool				_frameSelectionRequested = false;
	};
}
