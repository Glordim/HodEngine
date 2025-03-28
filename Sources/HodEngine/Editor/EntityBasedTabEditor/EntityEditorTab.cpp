#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/PrefabResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

namespace hod::editor
{
	/// @brief 
	EntityEditorTab::EntityEditorTab(std::shared_ptr<Asset> asset, const char* icon)
	: EditorTab(asset, icon)
	{
		_scene = new game::Scene();

		_world = new game::World();
		_world->AddScene(_scene);
	}

	/// @brief 
	EntityEditorTab::~EntityEditorTab()
	{
		_world->RemoveScene(_scene);

		delete _scene;
		delete _world;
	}

	/// @brief 
	void EntityEditorTab::ReloadScene()
	{
		_world->RemoveScene(_scene);
		_world->AddScene(_scene);
	}

	/// @brief 
	/// @return 
	game::World* EntityEditorTab::GetWorld() const
	{
		return _world;
	}

	/// @brief 
	/// @return 
	game::Scene* EntityEditorTab::GetCurrentScene() const
	{
		return _scene;
	}

	/// @brief 
	void EntityEditorTab::Play()
	{
		if (_playing == true)
		{
			return;
		}

		game::World* world = game::World::GetInstance();

		world->SetEditorPlaying(true);

		_playing = true;
	}

	/// @brief 
	void EntityEditorTab::Stop()
	{
		if (_playing == false)
		{
			return;
		}

		_playing = false;
		_paused = false;

		game::World* world = game::World::GetInstance();
		world->SetEditorPlaying(_playing);
		world->SetEditorPaused(_paused);
		world->Clear();
		ReloadScene();
	}
	
	/// @brief 
	void EntityEditorTab::Pause()
	{
		if (_paused == true)
		{
			return;
		}

		_paused = true;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void EntityEditorTab::Resume()
	{
		if (_paused == false)
		{
			return;
		}

		_paused = false;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void EntityEditorTab::PlayNextFrame()
	{
		Pause();
		game::World::GetInstance()->EditorNextFrame();
	}

	/// @brief 
	/// @return 
	bool EntityEditorTab::IsPlaying() const
	{
		return _playing;
	}

	/// @brief 
	/// @return 
	bool EntityEditorTab::IsPaused() const
	{
		return _paused;
	}

	/// @brief 
	/// @param asset 
	void EntityEditorTab::SetEntitySelection(std::shared_ptr<game::Entity> selection)
	{
		_entitySelection = selection;
	}

	/// @brief 
	/// @return 
	std::shared_ptr<game::Entity> EntityEditorTab::GetEntitySelection() const
	{
		return _entitySelection.lock();
	}
}
