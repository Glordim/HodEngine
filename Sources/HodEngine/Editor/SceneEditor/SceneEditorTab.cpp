#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/PrefabResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

namespace hod::editor
{
	/// @brief 
	SceneEditorTab::SceneEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_IMAGE_FILTER_HDR)
	{
		_scene = new game::Scene();

		if (asset != nullptr)
		{
			_scene->SetName(asset->GetName());

			Document document;
			DocumentReaderJson documentReader;
			if (documentReader.Read(document, asset->GetPath()) == false)
			{
				return; // todo message + bool
			}

			SceneImporter sceneImporter;
			if (asset->GetMeta()._importerType == sceneImporter.GetTypeName())
			{
				if (Serializer::Deserialize(_scene, document.GetRootNode()) == false)
				{
					return; // todo message + bool
				}
				asset->SetInstanceToSave(_scene, &_scene->GetReflectionDescriptorV());
			}
			else
			{
				std::shared_ptr<game::PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<game::PrefabResource>(asset->GetUid());
				_scene->SetNextLocalId(prefabResource->GetPrefab().GetNextLocalId());
				std::shared_ptr<game::Entity> prefabRootEntity = _scene->Instantiate(prefabResource);
				prefabRootEntity->SetPrefabResource(nullptr); // Invalid reference to PrefabResource to avoid serialization as PrefabInstance
				asset->SetInstanceToSave(_scene, &_scene->GetReflectionDescriptorV());
			}
		}

		_world = new game::World();
		_world->AddScene(_scene);
	}

	/// @brief 
	SceneEditorTab::~SceneEditorTab()
	{
		_world->RemoveScene(_scene);

		std::shared_ptr<Asset> asset = GetAsset();
		if (asset != nullptr)
		{
			asset->SetInstanceToSave(nullptr, nullptr);
			asset->ResetDirty();
		}
		delete _scene;
		delete _world;
	}

	/// @brief 
	void SceneEditorTab::CreateDefaultLayout()
	{
		//imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		HierachyWindow* hierarchyWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>(this);
		InspectorWindow* inspectorWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>(this);
		ViewportWindow* viewportWindow = imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>(this);

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool SceneEditorTab::DrawContent()
	{
		// todo override GetIdentifier ?
		/*
		if (_asset != nullptr && _assetWasDirty == true && _asset->IsDirty() == false)
		{
			_assetWasDirty = false;
			SetTitle(_asset->GetName());
		}
		*/
		//

		return true;
	}

	/// @brief 
	void SceneEditorTab::ReloadScene()
	{
		_world->RemoveScene(_scene);
		_world->AddScene(_scene);
	}

	/// @brief 
	/// @return 
	game::World* SceneEditorTab::GetWorld() const
	{
		return _world;
	}

	/// @brief 
	/// @return 
	game::Scene* SceneEditorTab::GetCurrentScene() const
	{
		return _scene;
	}

	/// @brief 
	void SceneEditorTab::Play()
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
	void SceneEditorTab::Stop()
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
	void SceneEditorTab::Pause()
	{
		if (_paused == true)
		{
			return;
		}

		_paused = true;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void SceneEditorTab::Resume()
	{
		if (_paused == false)
		{
			return;
		}

		_paused = false;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void SceneEditorTab::PlayNextFrame()
	{
		Pause();
		game::World::GetInstance()->EditorNextFrame();
	}

	/// @brief 
	/// @return 
	bool SceneEditorTab::IsPlaying() const
	{
		return _playing;
	}

	/// @brief 
	/// @return 
	bool SceneEditorTab::IsPaused() const
	{
		return _paused;
	}
}
