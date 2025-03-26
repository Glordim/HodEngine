#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

namespace hod::game
{
	class World;
	class Scene;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API SceneEditorTab : public EditorTab
	{
	public:
				SceneEditorTab(std::shared_ptr<Asset> asset);
				~SceneEditorTab() override;

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

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;

	private:

		game::World*				_world = nullptr;
		game::Scene*				_scene = nullptr;

		bool						_playing = false;
		bool						_paused = false;
	};
}
