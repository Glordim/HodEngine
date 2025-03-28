#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

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

		void							SetEntitySelection(std::shared_ptr<game::Entity> selection);
		std::shared_ptr<game::Entity>	GetEntitySelection() const;

	protected:

		game::World*				_world = nullptr;
		game::Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;

		std::weak_ptr<game::Entity>	_entitySelection;
	};
}
