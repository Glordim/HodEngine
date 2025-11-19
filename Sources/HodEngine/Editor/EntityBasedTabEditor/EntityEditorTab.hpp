#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <memory>

namespace hod::game
{
	class World;
	class Scene;
	class Entity;
}

namespace hod::editor
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

		game::World*	GetWorld() const;
		game::Scene*	GetCurrentScene() const;

		void			SetEntitySelection(game::Entity* selection);
		game::Entity*	GetEntitySelection() const;

	protected:
	
		void DrawMenuBar() override;
		
	protected:

		game::World*				_world = nullptr;

		game::World*				_editingWorld = nullptr;
		game::World*				_playingWorld = nullptr;

		game::Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;

		WeakPtr<game::Entity>		_entitySelection;
	};
}
