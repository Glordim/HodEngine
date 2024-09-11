#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/Prefab.hpp"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(InspectorWindow, "Inspector", true)

	/// @brief 
	void InspectorWindow::DrawContent()
	{
		Editor* editor = Editor::GetInstance();
		std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection().lock();
		if (sceneSelection != nullptr)
		{
			DrawSceneSelection(sceneSelection);
		}
		else
		{
			const AssetDatabase::FileSystemMapping* assetSelection = editor->GetAssetSelection();
			if (assetSelection != nullptr)
			{
				DrawAssetSelection(assetSelection);
			}
		}
	}

	/// @brief 
	/// @param selection 
	void InspectorWindow::DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection)
	{
		ImGui::Text("Asset");

		if (selection->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::shared_ptr<Asset> asset = selection->_asset;
			ImGui::TextUnformatted(asset->GetName().c_str()); // same line add button to open (and later VersionControl)
			ImGui::Separator();
			bool changed = false;
			ReflectionDescriptor* reflectionDescriptor = asset->GetMeta()._importerSettings->GetReflectionDescriptorV();
			ReflectionTraitImporterCustomEditor* componentCustomEditorTrait = reflectionDescriptor->FindTrait<ReflectionTraitImporterCustomEditor>();
			if (componentCustomEditorTrait != nullptr)
			{
				changed = componentCustomEditorTrait->GetCustomEditor()->OnDrawInspector(asset);
			}
			else
			{
				//changed = DrawDefaultInspector(asset->GetMeta()._importerSettings, reflectionDescriptor);
			}
			if (changed == true)
			{
				selection->_asset->SetDirty();
			}
		}
	}

	/// @brief 
	/// @param selection 
	void InspectorWindow::DrawSceneSelection(std::shared_ptr<game::Entity> selection)
	{
		game::PrefabUtility::EntityDiffs entityDiffs;

		game::Prefab* prefab = selection->GetPrefab();
		if (prefab != nullptr)
		{
			// todo don't do that in play mode
			game::PrefabUtility::CollectDiff(selection, entityDiffs);
			//

			if (ImGui::BeginChild("PrefabInstance", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle) == true)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
				ImGui::TextUnformatted(ICON_MDI_INFORMATION);
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Instance of");
				ImGui::SameLine();

				AssetDatabase* assetDatabase = AssetDatabase::GetInstance();
				std::shared_ptr<Asset> asset = assetDatabase->Find(prefab->GetUid());

				if (ImGui::Button(asset->GetName().c_str()))
				{
					AssetBrowserWindow* assetBrowserWindow = imgui::ImGuiManager::GetInstance()->FindWindow<AssetBrowserWindow>();
					if (assetBrowserWindow != nullptr)
					{
						assetBrowserWindow->PingAsset(*asset);
					}
				}

				// todo hide in play mode
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_PENCIL).x, 0.0f);
				ImGui::BeginDisabled(entityDiffs._diffs.empty());
				if (ImGui::Button(ICON_MDI_PENCIL))
				{
					ImGui::OpenPopup("EditOverrides");
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip) && entityDiffs._diffs.empty())
				{
					ImGui::SetTooltip("%s", "No override detected");
				}
				ImGui::EndDisabled();

				if (ImGui::BeginPopup("EditOverrides"))
				{
					if (ImGui::Button(ICON_MDI_RESTORE " Revert all"))
					{
						
					}
					ImGui::SameLine();
					if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Apply all"))
					{
						
					}
					ImGui::EndPopup();
				}
				//
			}
			ImGui::EndChild();
			ImGui::Separator();
		}

		ImGui::Text("Actor");

		char buffer[256] = { '\0' };
		std::strcpy(buffer, selection->GetName().c_str());
		if (ImGui::InputText("##Name", buffer, sizeof(buffer) - 1) == true)
		{
			selection->SetName(buffer);
			Editor::GetInstance()->MarkCurrentSceneAsDirty();
		}

		std::vector<std::weak_ptr<game::Component>> components = selection->GetComponents();
		for (const std::weak_ptr<game::Component>& component : components)
		{
			std::shared_ptr<game::Component> componentLock = component.lock();
			if (componentLock != nullptr)
			{
				/*
				bool hasOverride = false;
				std::vector<hod::game::PrefabUtility::EntityDiffs::Diff*> componentDiffs;
				for (auto diff : entityDiffs._diffs)
				{
					if (diff->_instance == componentLock.get())
					{
						hasOverride = true;
						componentDiffs.push_back(diff);
					}
				}
				if (hasOverride == true)
				{
					float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
				}
				*/
				//bool opened = ImGui::CollapsingHeader(componentLock->GetMetaTypeName(), ImGuiTreeNodeFlags_DefaultOpen);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::SetCursorPosX(0.0f);
				ImGui::PopStyleVar(1);

				ImVec2 min = ImGui::GetCursorScreenPos();
				ImVec2 max = min;
				max.x += ImGui::GetWindowWidth();
				ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				ImGui::SetCursorPosX(0.0f);
				ImGui::Separator();
				ImGui::SetCursorPosX(0.0f);
				ImGui::PopStyleVar(2);

				min = ImGui::GetCursorScreenPos();
				max = min;
				max.x += ImGui::GetWindowWidth();
				max.y += ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 4;
				ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));

				min.y += ImGui::GetStyle().FramePadding.y;
				ImGui::SetCursorScreenPos(min);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ArrowButton("CollapseComponent", ImGuiDir_Down);
				ImGui::PopStyleColor(4);

				bool opened = true;
				ImGui::SameLine();
				ImGui::TextUnformatted(ICON_MDI_PUZZLE);
				ImGui::SameLine();
				ImGui::TextUnformatted(componentLock->GetMetaTypeName());

				/*
				if (ImGui::BeginPopupContextItem() == true)
				{
					if (ImGui::Button("Delete") == true)
					{
						selection->RemoveComponent(componentLock);
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
					ImGui::EndPopup();
				}
				*/

				if (opened == true)
				{
					EditorReflectedObject reflectedObject(componentLock);

					bool changed = false;
					ReflectionDescriptor* reflectionDescriptor = componentLock->GetReflectionDescriptorV();
					ReflectionTraitComponentCustomEditor* componentCustomEditorTrait = reflectionDescriptor->FindTrait<ReflectionTraitComponentCustomEditor>();
					if (componentCustomEditorTrait != nullptr)
					{
						changed = componentCustomEditorTrait->GetCustomEditor()->OnDrawInspector(componentLock.get(), reflectionDescriptor);
					}
					else
					{
						changed = DrawDefaultInspector(reflectedObject);
					}
					if (changed == true)
					{
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
				}
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(0.0f);
		ImGui::PopStyleVar(1);
		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = min;
		max.x += ImGui::GetWindowWidth();
		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));
		ImGui::Spacing();

		ImVec2 addComponentButtonSize = ImGui::CalcTextSize("  Add component  ") + ImVec2(35.0f, 15.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addComponentButtonSize.x * 0.5f));
		if (ImGui::Button("  Add component  ", addComponentButtonSize) == true)
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent") == true)
		{
			for (const auto& componentDescriptorPair : game::ComponentFactory::GetInstance()->GetAllDescriptors())
			{
				const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;

				ReflectionTraitDisplayName* displayNameTrait = componentDescriptor.FindTrait<ReflectionTraitDisplayName>();
				if (ImGui::MenuItem(displayNameTrait->GetValue().c_str()) == true)
				{
					selection->AddComponent(componentDescriptor, Editor::GetInstance()->IsPlaying());
					Editor::GetInstance()->MarkCurrentSceneAsDirty();
				}
			}

			ImGui::EndPopup();
		}
	}

	/// @brief 
	/// @param object 
	bool InspectorWindow::DrawDefaultInspector(EditorReflectedObject& reflectedObject)
	{
		return PropertyDrawer::DrawDescriptor(reflectedObject);
	}
}
