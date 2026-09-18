#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"

#include <algorithm>
#include <cmath>

namespace hod::inline editor
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
		Entity* sceneSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();
		if (sceneSelection != nullptr)
		{
			DrawSceneSelection(sceneSelection);
		}
	}

	/// @brief
	/// @param selection
	void InspectorWindow::DrawSceneSelection(Entity* selection)
	{
		Vector<PrefabUtility::PrefabOverride> overrides;

		std::shared_ptr<PrefabResource> prefabResource = selection->GetPrefabResource();
		if (prefabResource != nullptr)
		{
			// todo don't do that in play mode
			PrefabUtility::CollectPrefabOverride(selection, overrides);
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
			GetOwner()->MarkAsDirty();
		}

		for (Component* component : selection->GetComponents())
		{
			if (component != nullptr)
			{
				/*
				bool hasOverride = false;
				Vector<hod::PrefabUtility::EntityDiffs::Diff*> componentDiffs;
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
				// bool opened = ImGui::CollapsingHeader(componentLock->GetRttiTypeName(), ImGuiTreeNodeFlags_DefaultOpen);

				bool enabled = component->GetEnabled();
				bool removeRequested = false;
				bool opened = DrawHelper::BeginInspectorBlock((const void*)(uintptr_t)component->GetReflectionDescriptorV().GetType(), ICON_MDI_PUZZLE, component->GetReflectionDescriptorV().GetDisplayName().CStr(), &enabled, &removeRequested);
				if (enabled != component->GetEnabled())
				{
					component->SetEnabled(enabled);
				}
				if (removeRequested)
				{
					selection->RemoveComponent(component);
					GetOwner()->MarkAsDirty();
					opened = false;
				}

				if (opened == true)
				{
					Component*            sourceComponent = PrefabUtility::GetCorrespondingComponent(component);
					EditorReflectedObject reflectedObject(component, &component->GetReflectionDescriptorV(), sourceComponent, this);

					bool                   changed = false;
					ReflectionDescriptor&  reflectionDescriptor = component->GetReflectionDescriptorV();
					CustomComponentDrawer* drawer = CustomComponentDrawerRegistry::Find(reflectionDescriptor);
					if (drawer != nullptr)
					{
						changed = drawer->OnDrawInspector(reflectedObject);
					}
					else
					{
						changed = DrawDefaultInspector(reflectedObject);
					}
					if (changed == true)
					{
						GetOwner()->MarkAsDirty();
					}
				}
				DrawHelper::EndInspectorBlock();
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
				for (const auto& componentDescriptorPair : ComponentFactory::GetInstance()->GetAllDescriptors())
				{
					const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;
					const String&               displayName = componentDescriptor.GetDisplayName();
					if (MatchSearch(displayName, strName))
					{
						if (ImGui::MenuItem(displayName.CStr()) == true)
						{
							selection->AddComponent(componentDescriptor);
							GetOwner()->MarkAsDirty();
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
