#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/HierachyWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/InspectorWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWindow.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Helper.hpp>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/PrefabResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Game/SceneSerializer.hpp>

#include <algorithm>
#include <cstring>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

namespace hod::inline editor
{
	/// @brief 
	EntityEditorTab::EntityEditorTab(std::shared_ptr<Asset> asset, const char* icon)
	: EditorTab(asset, icon)
	{
		_scene = DefaultAllocator::GetInstance().New<Scene>();

		_editingWorld = DefaultAllocator::GetInstance().New<game::World>();
		_editingWorld->Init();
		_editingWorld->SetDrawnOnMainWindow(false); // Drawn by its viewports
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
	Scene* EntityEditorTab::GetCurrentScene() const
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
		_playingWorld->SetDrawnOnMainWindow(false); // Drawn by its viewports
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
	void EntityEditorTab::SetEntitySelection(Entity* selection)
	{
		_entitySelection = selection;
	}

	/// @brief 
	/// @return 
	Entity* EntityEditorTab::GetEntitySelection() const
	{
		return _entitySelection.Get();
	}

	/// @brief
	/// @return
	Entity* EntityEditorTab::GetRootEntity() const
	{
		return nullptr;
	}

	/// @brief
	/// @param entity
	/// @return
	bool EntityEditorTab::IsRootEntity(const Entity* entity) const
	{
		return entity != nullptr && entity == GetRootEntity();
	}

	/// @brief
	/// @return
	Entity* EntityEditorTab::GetDefaultParent() const
	{
		return GetRootEntity();
	}

	/// @brief
	/// @param entity
	/// @param parent
	/// @return
	bool EntityEditorTab::CanReparent(const Entity* entity, const Entity* parent) const
	{
		if (entity == nullptr || IsRootEntity(entity))
		{
			return false;
		}
		if (parent == nullptr)
		{
			return GetRootEntity() == nullptr;
		}
		for (const Entity* ancestor = parent; ancestor != nullptr; ancestor = ancestor->GetParent().Lock())
		{
			if (ancestor == entity)
			{
				return false;
			}
		}
		return true;
	}

	/// @brief
	/// @param prefab
	/// @return
	bool EntityEditorTab::CanInstantiatePrefab(const UID& prefab) const
	{
		(void)prefab;
		return true;
	}

	/// @brief
	/// @param entity
	void EntityEditorTab::DeleteEntity(Entity* entity)
	{
		if (entity == nullptr || IsRootEntity(entity))
		{
			return;
		}

		for (Entity* selected = GetEntitySelection(); selected != nullptr; selected = selected->GetParent().Lock())
		{
			if (selected == entity)
			{
				SetEntitySelection(nullptr);
				break;
			}
		}

		_world->DestroyEntity(entity);
		MarkAsDirty("Delete");
	}

	namespace
	{
		/// @brief A copy has the ids of its source: cleared, to be given new ones when saved (see SceneSerializer). Not inside an
		/// instance, whose ids are its prefab's
		void ClearLocalIds(Entity* entity)
		{
			entity->SetLocalId(0);
			if (entity->GetPrefabResource() != nullptr)
			{
				return;
			}
			for (Component* component : entity->GetComponents())
			{
				component->SetLocalId(0);
			}
			for (const WeakEntity& child : entity->GetChildren())
			{
				if (Entity* childEntity = child.Lock())
				{
					ClearLocalIds(childEntity);
				}
			}
		}
	}

	/// @brief
	/// @param entity
	/// @return
	Entity* EntityEditorTab::DuplicateEntity(Entity* entity)
	{
		if (entity == nullptr || IsRootEntity(entity) || entity->GetScene() == nullptr)
		{
			return nullptr;
		}

		Entity* copy = entity->GetScene()->Instantiate(entity);
		if (copy == nullptr)
		{
			return nullptr;
		}
		ClearLocalIds(copy);

		Entity* parent = entity->GetParent().Lock();
		if (parent != nullptr)
		{
			copy->SetParent(parent);
			copy->SetSiblingIndex(entity->GetSiblingIndex() + 1);
		}

		SetEntitySelection(copy);
		MarkAsDirty("Duplicate");
		return copy;
	}

	namespace
	{
		// The clipboard's text starts with it (see CopyEntities)
		constexpr const char* ClipboardFormat = "HodEngine.Entities";

		/// @brief entities but those inside another one of them (which come with it)
		Vector<Entity*> GetSubtrees(const Vector<Entity*>& entities)
		{
			Vector<Entity*> subtrees;
			for (Entity* entity : entities)
			{
				if (entity == nullptr)
				{
					continue;
				}
				bool inside = false;
				for (Entity* ancestor = entity->GetParent().Lock(); ancestor != nullptr && inside == false; ancestor = ancestor->GetParent().Lock())
				{
					inside = std::find(entities.CBegin(), entities.CEnd(), ancestor) != entities.CEnd();
				}
				if (inside == false && std::find(subtrees.Begin(), subtrees.End(), entity) == subtrees.End())
				{
					subtrees.PushBack(entity);
				}
			}
			return subtrees;
		}

		bool IsInside(const Entity* entity, const Entity* ancestor)
		{
			for (; entity != nullptr; entity = entity->GetParent().Lock())
			{
				if (entity == ancestor)
				{
					return true;
				}
			}
			return false;
		}
	}

	/// @brief
	/// @param entities
	/// @return
	bool EntityEditorTab::CopyEntities(const Vector<Entity*>& entities)
	{
		CancelCut();

		Vector<Entity*> subtrees = GetSubtrees(entities);
		if (subtrees.Empty())
		{
			return false;
		}

		// The Format first: what CanPaste looks for
		Document document;
		document.GetRootNode().AddChild("Format").SetString(ClipboardFormat);
		DocumentNode& entitiesNode = document.GetRootNode().AddChild("Entities");
		for (Entity* entity : subtrees)
		{
			// The ids missing yet are given from the scene's, as a save would
			Scene*   scene = entity->GetScene();
			uint64_t nextLocalId = scene != nullptr ? scene->GetNextLocalId() : 1;
			SceneSerializer sceneSerializer;
			if (sceneSerializer.SerializeEntity(entity, true, entitiesNode, nextLocalId) == false)
			{
				return false;
			}
			if (scene != nullptr)
			{
				scene->SetNextLocalId(nextLocalId);
			}
		}

		String             text;
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, text) == false)
		{
			return false;
		}
		ImGui::SetClipboardText(text.CStr());
		return true;
	}

	/// @brief
	/// @param entities
	/// @return
	bool EntityEditorTab::CutEntities(const Vector<Entity*>& entities)
	{
		Vector<Entity*> subtrees = GetSubtrees(entities);
		for (Entity* entity : subtrees)
		{
			if (IsRootEntity(entity))
			{
				return false;
			}
		}
		if (CopyEntities(subtrees) == false)
		{
			return false;
		}

		for (Entity* entity : subtrees)
		{
			_cutEntities.PushBack(entity);
		}
		// Read back rather than kept from CopyEntities: what the clipboard gives back (encoding round trip) is what a paste compares
		const char* text = ImGui::GetClipboardText();
		_cutClipboardText = (text != nullptr) ? text : "";
		return true;
	}

	/// @brief
	void EntityEditorTab::CancelCut()
	{
		_cutEntities.Clear();
		_cutClipboardText.Clear();
	}

	/// @brief
	/// @param entity
	/// @return
	bool EntityEditorTab::IsCut(const Entity* entity) const
	{
		for (const WeakPtr<Entity>& cutEntity : _cutEntities)
		{
			if (cutEntity.Get() != nullptr && IsInside(entity, cutEntity.Get()))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @return
	bool EntityEditorTab::HasCutEntities() const
	{
		return _cutEntities.Empty() == false;
	}

	/// @brief
	/// @return
	bool EntityEditorTab::CanPaste() const
	{
		const char* text = ImGui::GetClipboardText();
		return text != nullptr && std::strstr(text, ClipboardFormat) != nullptr;
	}

	/// @brief
	/// @param parent
	/// @param index
	/// @return
	Vector<Entity*> EntityEditorTab::PasteEntities(Entity* parent, uint32_t index)
	{
		Vector<Entity*> pasted;
		if (CanPaste() == false || (parent == nullptr && GetRootEntity() != nullptr))
		{
			return pasted;
		}
		const char* text = ImGui::GetClipboardText();

		// Entities cut here are moved, as long as the clipboard still holds what the cut put there (nothing copied since, here or elsewhere)
		if (_cutEntities.Empty() == false && _cutClipboardText == text)
		{
			Vector<Entity*> cutEntities;
			for (const WeakPtr<Entity>& cutEntity : _cutEntities)
			{
				if (cutEntity.Get() != nullptr)
				{
					cutEntities.PushBack(cutEntity.Get());
				}
			}
			cutEntities = GetSubtrees(cutEntities);
			if (cutEntities.Empty() == false)
			{
				for (Entity* entity : cutEntities)
				{
					if (CanReparent(entity, parent) == false)
					{
						return pasted; // into one of them: nothing done, still cut
					}
				}
				for (Entity* entity : cutEntities)
				{
					entity->SetParent(parent);
					if (parent != nullptr && index != UINT32_MAX)
					{
						entity->SetSiblingIndex(index++);
					}
					pasted.PushBack(entity);
				}
				CancelCut();
				SetEntitySelection(pasted[pasted.Size() - 1]);
				MarkAsDirty("Paste");
				return pasted;
			}
		}
		CancelCut();

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, text, std::strlen(text)) == false)
		{
			return pasted;
		}
		const DocumentNode* entitiesNode = document.GetRootNode().GetChild("Entities");
		if (entitiesNode == nullptr)
		{
			return pasted;
		}

		// Not an instance of a prefab into itself
		for (const DocumentNode* entityNode = entitiesNode->GetFirstChild(); entityNode != nullptr; entityNode = entityNode->GetNextSibling())
		{
			const DocumentNode* prefabInstanceNode = entityNode->GetChild("PrefabInstance");
			const DocumentNode* uidNode = prefabInstanceNode != nullptr ? prefabInstanceNode->GetChild("UID") : nullptr;
			if (uidNode != nullptr)
			{
				UID prefab;
				Serializer::Deserialize(prefab, *uidNode);
				if (CanInstantiatePrefab(prefab) == false)
				{
					OUTPUT_ERROR("Can't paste: a prefab can't contain itself, the copied entities hold an instance of it");
					return pasted;
				}
			}
		}

		Scene* scene = parent != nullptr ? parent->GetScene() : (_world->GetScenes().Empty() ? nullptr : _world->GetScenes()[0]);
		if (scene == nullptr)
		{
			return pasted;
		}

		for (Entity* entity : scene->Instantiate(*entitiesNode))
		{
			ClearLocalIds(entity); // Given new ones on save, the copied ones may exist here already
			if (parent != nullptr)
			{
				entity->SetParent(parent);
				if (index != UINT32_MAX)
				{
					entity->SetSiblingIndex(index++);
				}
			}
			pasted.PushBack(entity);
		}

		if (pasted.Empty() == false)
		{
			SetEntitySelection(pasted[pasted.Size() - 1]);
			MarkAsDirty("Paste");
		}
		return pasted;
	}

	/// @brief
	/// @return
	Vector<Entity*> EntityEditorTab::PasteNextToSelection()
	{
		Entity* selection = GetEntitySelection();
		if (selection == nullptr || IsRootEntity(selection))
		{
			return PasteEntities(GetDefaultParent());
		}
		Entity* parent = selection->GetParent().Lock();
		return PasteEntities(parent, parent != nullptr ? selection->GetSiblingIndex() + 1 : UINT32_MAX);
	}

	/// @brief
	/// @return
	Vector<Entity*> EntityEditorTab::PasteInSelection()
	{
		Entity* selection = GetEntitySelection();
		return PasteEntities(selection != nullptr ? selection : GetDefaultParent());
	}

	/// @brief
	void EntityEditorTab::HandleClipboardShortcuts()
	{
		Entity* selection = GetEntitySelection();
		if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C) && selection != nullptr)
		{
			CopyEntities({selection});
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_X) && selection != nullptr)
		{
			CutEntities({selection});
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
		{
			PasteNextToSelection();
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_V))
		{
			PasteInSelection();
		}
		else if (_cutEntities.Empty() == false && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
		{
			CancelCut();
		}
	}

	/// @brief
	void EntityEditorTab::RequestFrameSelection()
	{
		_frameSelectionRequested = true;
	}

	/// @brief
	/// @return
	bool EntityEditorTab::ConsumeFrameSelectionRequest()
	{
		bool requested = _frameSelectionRequested;
		_frameSelectionRequested = false;
		return requested;
	}
}
