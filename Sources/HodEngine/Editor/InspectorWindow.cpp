#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"

#include <algorithm>
#include <cmath>

namespace hod::editor
{
	// TODO move in reusable header
	bool MatchSearch(const hod::String& text, const hod::String& pattern)
	{
		if (pattern.Empty() == true)
		{
			return true;
		}

		if (text.Size() < pattern.Size())
		{
			return false;
		}

		for (uint32_t i = 0; i <= text.Size() - pattern.Size(); ++i)
		{
			bool match = true;

			for (uint32_t j = 0; j < pattern.Size(); ++j)
			{
				unsigned char a = static_cast<unsigned char>(text[i + j]);
				unsigned char b = static_cast<unsigned char>(pattern[j]);

				if (std::tolower(a) != std::tolower(b))
				{
					match = false;
					break;
				}
			}

			if (match)
			{
				return true;
			}
		}

		return false;
	}

	//

	DESCRIBE_REFLECTED_CLASS(InspectorWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	InspectorWindow::InspectorWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
	}

	/// @brief
	void InspectorWindow::DrawContent()
	{
		game::Entity* sceneSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();
		if (sceneSelection != nullptr)
		{
			DrawSceneSelection(sceneSelection);
		}
	}

	/// @brief
	/// @param selection
	void InspectorWindow::DrawSceneSelection(game::Entity* selection)
	{
		Vector<game::PrefabUtility::PrefabOverride> overrides;

		std::shared_ptr<game::PrefabResource> prefabResource = selection->GetPrefabResource();
		if (prefabResource != nullptr)
		{
			// todo don't do that in play mode
			game::PrefabUtility::CollectPrefabOverride(selection, overrides);
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

				AssetDatabase*         assetDatabase = AssetDatabase::GetInstance();
				std::shared_ptr<Asset> asset = assetDatabase->Find(prefabResource->GetUid());

				if (ImGui::Button(asset->GetName().CStr()))
				{
					Editor::GetInstance()->PingAsset(asset);
				}

				// todo hide in play mode
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_PENCIL).x, 0.0f);
				ImGui::BeginDisabled(overrides.Empty());
				if (ImGui::Button(ICON_MDI_PENCIL))
				{
					ImGui::OpenPopup("EditOverrides");
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip) && overrides.Empty())
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

		bool active = selection->GetActive();
		if (ImGui::Checkbox("##Active", &active))
		{
			selection->SetActive(active);
		}
		ImGui::SameLine();

		char buffer[256] = {'\0'};
		std::strcpy(buffer, selection->GetName().CStr());
		ImGui::SetNextItemWidth(-1);
		if (ImGui::InputText("##Name", buffer, sizeof(buffer) - 1) == true)
		{
			selection->SetName(buffer);
			GetOwner()->MarkAssetAsDirty();
		}

		for (game::Component* component : selection->GetComponents())
		{
			if (component != nullptr)
			{
				/*
				bool hasOverride = false;
				Vector<hod::game::PrefabUtility::EntityDiffs::Diff*> componentDiffs;
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
				    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetWindowPos().x + 2.0f,
				ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
				}
				*/
				// bool opened = ImGui::CollapsingHeader(componentLock->GetMetaTypeName(), ImGuiTreeNodeFlags_DefaultOpen);

				ImGui::PushID(component->GetMetaTypeName());
				if (ImGui::BeginChild("Component", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
					ImGui::SetCursorPosX(0.0f);
					ImGui::SetCursorPosY(0.0f);
					ImGui::PopStyleVar(1);

					ImVec2 min = ImGui::GetCursorScreenPos();
					ImVec2 max = min;
					max.x += ImGui::GetWindowWidth();
					// ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));

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

					bool collapsed = ImGui::GetCurrentWindow()->DC.StateStorage->GetInt(ImGui::GetID("Collapsed"), false);

					if (ImGui::ArrowButton("CollapseComponent", collapsed == false ? ImGuiDir_Down : ImGuiDir_Right))
					{
						collapsed = !collapsed;
						ImGui::GetCurrentWindow()->DC.StateStorage->SetInt(ImGui::GetID("Collapsed"), collapsed);
					}
					ImGui::PopStyleColor(4);

					ImGui::SameLine();
					bool enabled = component->GetEnabled();
					if (ImGui::Checkbox("##Enabled", &enabled))
					{
						component->SetEnabled(enabled);
					}

					ImGui::BeginDisabled(enabled == false);

					bool opened = (collapsed == false);
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(ICON_MDI_PUZZLE);
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(component->GetReflectionDescriptorV().GetDisplayName().CStr());

					ImGui::EndDisabled();

					ImGui::SameLine(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_CLOSE).x + 10.0f, 0.0f);
					ImVec2 buttonPos = ImGui::GetCursorScreenPos();
					ImVec2 mousePos = ImGui::GetMousePos();

					float distance = (float)std::sqrt(std::pow(buttonPos.x - mousePos.x, 2) + std::pow(buttonPos.y - mousePos.y, 2));
					distance = std::clamp(distance, 15.0f, 40.0f);
					float alpha = 1.0f - ((distance - 15.0f) / (40.0f - 15.0f));

					ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
					textColor.w = alpha;

					ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
					buttonColor.w = alpha;

					ImVec4 borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
					borderColor.w = alpha;

					ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
					ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					bool mustBeDelete = (ImGui::Button(ICON_MDI_CLOSE));
					ImGui::PopStyleColor(4);
					if (mustBeDelete)
					{
						selection->RemoveComponent(component);
						GetOwner()->MarkAssetAsDirty();
					}

					if (ImGui::IsWindowHovered())
					{
					}

					/*
					if (ImGui::BeginPopupContextItem() == true)
					{
					    if (ImGui::Button("Delete") == true)
					    {
					        selection->RemoveComponent(componentLock);
					        GetOwner()->MarkAssetAsDirty();
					    }
					    ImGui::EndPopup();
					}
					*/

					{
						min.y = max.y;
						max.x += ImGui::GetWindowWidth();
						ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));
					}

					if (opened == true)
					{
						ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 2.0f));

						game::Component*      sourceComponent = game::PrefabUtility::GetCorrespondingComponent(component);
						EditorReflectedObject reflectedObject(component, &component->GetReflectionDescriptorV(), sourceComponent, this);

						bool                                  changed = false;
						ReflectionDescriptor&                 reflectionDescriptor = component->GetReflectionDescriptorV();
						ReflectionTraitComponentCustomEditor* componentCustomEditorTrait = reflectionDescriptor.FindTrait<ReflectionTraitComponentCustomEditor>();
						if (componentCustomEditorTrait != nullptr)
						{
							changed = componentCustomEditorTrait->GetCustomEditor()->OnDrawInspector(reflectedObject);
						}
						else
						{
							changed = DrawDefaultInspector(reflectedObject);
						}
						if (changed == true)
						{
							GetOwner()->MarkAssetAsDirty();
						}
					}
					else
					{
						ImVec2 pos = ImGui::GetCursorScreenPos();
						pos.y = max.y;
						ImGui::SetCursorScreenPos(pos);
					}
				}
				ImGui::Dummy(ImVec2(0, 0)); // todo
				ImGui::EndChild();

				ImGui::PopID();
			}
		}

		ImGui::Spacing();

		ImVec2 addComponentButtonSize = ImGui::CalcTextSize("  Add component  ") + ImVec2(35.0f, 15.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addComponentButtonSize.x * 0.5f));
		if (ImGui::Button("  Add component  ", addComponentButtonSize) == true)
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent") == true)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
			ImGui::SameLine();

			static char   nameBuffer[256] = {'\0'};
			static String strName;
			if (ImGui::IsWindowAppearing())
			{
				nameBuffer[0] = '\0';
				strName.Clear();
				ImGui::SetKeyboardFocusHere();
			}
			if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
			{
				strName = nameBuffer;
			}
			ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.5f, 0.5f));
			ImGui::SeparatorText("Components");
			ImGui::PopStyleVar();

			if (ImGui::BeginChild("ComponentList", ImVec2(0.0f, 350.0f), ImGuiChildFlags_FrameStyle))
			{
				for (const auto& componentDescriptorPair : game::ComponentFactory::GetInstance()->GetAllDescriptors())
				{
					const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;
					const String&               displayName = componentDescriptor.GetDisplayName();
					if (MatchSearch(displayName, strName))
					{
						if (ImGui::MenuItem(displayName.CStr()) == true)
						{
							selection->AddComponent(componentDescriptor);
							GetOwner()->MarkAssetAsDirty();
							ImGui::CloseCurrentPopup();
						}
					}
				}
			}
			ImGui::EndChild();
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
