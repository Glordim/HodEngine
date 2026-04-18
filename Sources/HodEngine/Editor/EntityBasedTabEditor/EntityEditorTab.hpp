#pragma once
#include "HodEngine/Editor/Export.hpp"

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
	class HOD_EDITOR_API EntityEditorTab : public EditorTab
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

		World*	GetWorld() const;
		Scene*	GetCurrentScene() const;

		void			SetEntitySelection(Entity* selection);
		Entity*	GetEntitySelection() const;

	protected:
	
		void DrawMenuBar() override;
		
	protected:

		World*				_world = nullptr;

		World*				_editingWorld = nullptr;
		World*				_playingWorld = nullptr;

		Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;

		WeakPtr<Entity>		_entitySelection;
	};
}
