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
#include <HodEngine/ImGui/Helper.hpp>

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
		_scene = DefaultAllocator::GetInstance().New<game::Scene>();

		_editingWorld = DefaultAllocator::GetInstance().New<game::World>();
		_editingWorld->Init();
		_editingWorld->DisableDrawJob();
		_editingWorld->AddScene(_scene);
		_editingWorld->ProcessActication();
		_world = _editingWorld;
	}

	/// @brief 
	EntityEditorTab::~EntityEditorTab()
	{
		_editingWorld->RemoveScene(_scene);

		DefaultAllocator::GetInstance().Delete(_scene);
		DefaultAllocator::GetInstance().Delete(_editingWorld);
	}

	/// @brief 
	void EntityEditorTab::DrawMenuBar()
	{
		//const ImGuiStyle& style = ImGui::GetStyle();
		//float groupWidth = CalculateButtonSize(ICON_MDI_PLAY).x + style.ItemSpacing.x + CalculateButtonSize(ICON_MDI_PAUSE).x + style.ItemSpacing.x + CalculateButtonSize(ICON_MDI_SKIP_NEXT).x;
		//ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - groupWidth * 0.5f);

		ImGui::SameLine();

		if (_playing == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_STOP, ImVec2(0, 32));
			ImGui::PopStyleColor();
			if (pressed == true)
			{
				Stop();
			}
		}
		else if (ImGui::Button(ICON_MDI_PLAY, ImVec2(0, 32)) == true)
		{
			Play();
		}

		ImGui::SameLine();

		if (_paused == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 32));
			ImGui::PopStyleColor();
			if (pressed == true)
			{
				Resume();
			}
		}
		else if (ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 32)) == true)
		{
			Pause();
		}

		ImGui::SameLine();

		ImGui::BeginDisabled(_playing == false);
		if (ImGui::Button(ICON_MDI_SKIP_NEXT, ImVec2(0, 32)) == true)
		{
			PlayNextFrame();
		}
		ImGui::EndDisabled();
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

		_playing = true;

		_playingWorld = _editingWorld->Clone();
		_playingWorld->DisableDrawJob();
		_world = _playingWorld;

		_playingWorld->SetEditorPlaying(_playing);
		_playingWorld->SetEditorPaused(_paused);
		_playingWorld->ProcessActication();
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

		_world = _editingWorld;
		DefaultAllocator::GetInstance().Delete(_playingWorld);
		_playingWorld = nullptr;

		//_world->SetEditorPlaying(_playing);
		//_world->SetEditorPaused(_paused);
		//_world->Clear();
		//ReloadScene();
	}

	/// @brief 
	void EntityEditorTab::Pause()
	{
		if (_paused == true)
		{
			return;
		}

		_paused = true;

		_world->SetEditorPaused(_paused);
	}

	/// @brief 
	void EntityEditorTab::Resume()
	{
		if (_paused == false)
		{
			return;
		}

		_paused = false;

		_world->SetEditorPaused(_paused);
	}

	/// @brief 
	void EntityEditorTab::PlayNextFrame()
	{
		Pause();
		_world->EditorNextFrame();
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
	void EntityEditorTab::SetEntitySelection(game::Entity* selection)
	{
		_entitySelection = selection;
	}

	/// @brief 
	/// @return 
	game::Entity* EntityEditorTab::GetEntitySelection() const
	{
		return _entitySelection.Get();
	}
}
