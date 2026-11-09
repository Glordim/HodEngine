#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <memory>

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
