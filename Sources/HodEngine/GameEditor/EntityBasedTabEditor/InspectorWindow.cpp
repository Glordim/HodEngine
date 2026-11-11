#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/InspectorWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/GameEditor/ComponentDisplayName.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

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
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include <HodEngine/Core/Math.hpp>
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/Components/MissingComponent.hpp"

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
		SetTitle("Inspector");
	}

	/// @brief
	void InspectorWindow::DrawContent()
	{
		// The active entity only (the last selected)
		Entity* sceneSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();
		if (sceneSelection != nullptr)
		{
			uint32_t selectedCount = GetOwner<EntityEditorTab>()->GetSelectedEntities().Size();
			if (selectedCount > 1)
			{
				ImGui::TextDisabled("%u entities selected: the last one shown", selectedCount);
			}
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
			GetOwner()->MarkAsDirty(active ? "Activate" : "Deactivate");
		}
		ImGui::SameLine();

		// A prefab's root entity is named after the asset
		bool isRoot = GetOwner<EntityEditorTab>()->IsRootEntity(selection);
		char buffer[256] = {'\0'};
		std::strncpy(buffer, selection->GetName().CStr(), sizeof(buffer) - 1);
		ImGui::SetNextItemWidth(-1);
		ImGui::BeginDisabled(isRoot);
		if (ImGui::InputText("##Name", buffer, sizeof(buffer) - 1) == true)
		{
			selection->SetName(buffer);
			GetOwner()->MarkAsDirty("Rename");
		}
		ImGui::EndDisabled();
		if (isRoot && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Named after the asset: rename the asset to rename it");
		}

		Component* componentToRemove = nullptr; // After the loop: removing deletes it, and changes the list being drawn
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
				bool opened = DrawHelper::BeginInspectorBlock((const void*)(uintptr_t)component->GetReflectionDescriptorV().GetType(), ICON_MDI_PUZZLE, GetComponentDisplayName(component->GetReflectionDescriptorV()).CStr(), &enabled, &removeRequested);
				if (enabled != component->GetEnabled())
				{
					component->SetEnabled(enabled);
					GetOwner()->MarkAsDirty(fmt::format("{} {}", enabled ? "Enable" : "Disable", GetComponentDisplayName(component->GetReflectionDescriptorV())));
				}
				if (removeRequested)
				{
					componentToRemove = component;
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
						GetOwner()->MarkAsDirty(fmt::format("Edit {}", GetComponentDisplayName(reflectionDescriptor)));
					}
				}
				DrawHelper::EndInspectorBlock();
			}
		}
		if (componentToRemove != nullptr)
		{
			std::string label = fmt::format("Remove {}", GetComponentDisplayName(componentToRemove->GetReflectionDescriptorV()));
			selection->RemoveComponent(componentToRemove);
			GetOwner()->MarkAsDirty(label);
		}

		ImGui::Spacing();

		ImVec2 addComponentButtonSize = ImGui::CalcTextSize("  Add component  ") + ImVec2(35.0f, 15.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addComponentButtonSize.x * 0.5f));
		if (ImGui::Button("  Add component  ", addComponentButtonSize) == true)
		{
			ImGui::OpenPopup("AddComponent");
		}
		DrawAddComponentPopup(selection);
	}

	namespace
	{
		/// @brief The arrows pressed in the search field (the history keys of an InputText): counted in UserData, an int32_t
		int AddComponentSearchCallback(ImGuiInputTextCallbackData* data)
		{
			if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
			{
				*static_cast<int32_t*>(data->UserData) += (data->EventKey == ImGuiKey_UpArrow) ? -1 : 1;
			}
			return 0;
		}
	}

	/// @brief The list of the components that can be added, filtered by a search. Driven with the keyboard too: the arrows move the
	/// highlighted result, Enter adds it, Escape closes. The components the entity already has are greyed out (one of each type, see
	/// Entity::AddComponent).
	/// @param selection
	void InspectorWindow::DrawAddComponentPopup(Entity* selection)
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
		if (ImGui::BeginPopup("AddComponent") == false)
		{
			return;
		}

		bool appearing = ImGui::IsWindowAppearing();
		if (appearing)
		{
			_addComponentSearch[0] = '\0';
			_addComponentHighlight = 0;
			_addComponentScrollToHighlight = true;
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
		ImGui::SameLine();
		if (appearing)
		{
			ImGui::SetKeyboardFocusHere();
		}
		_addComponentMove = 0;
		ImGui::SetNextItemWidth(-FLT_MIN);
		bool enterPressed = ImGui::InputTextWithHint("##Search", "Search", _addComponentSearch, sizeof(_addComponentSearch),
		                                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, &AddComponentSearchCallback,
		                                             &_addComponentMove);
		bool searchEdited = ImGui::IsItemEdited();

		// The components that can be added (not the abstract ones, nor the hidden ones: bases not meant to be used alone, nor the
		// placeholder of an unknown one), by name, those matching the search
		struct Entry
		{
			const ReflectionDescriptor* _descriptor;
			String                      _name;
			bool                        _enabled; // not on the entity yet
		};
		Vector<Entry> entries;
		String        search(_addComponentSearch);
		for (const auto& componentDescriptorPair : ComponentFactory::GetInstance()->GetAllDescriptors())
		{
			const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;
			if (componentDescriptor.CanCreateInstance() == false || componentDescriptor.GetType() == MissingComponent::GetReflectionDescriptor().GetType() ||
			    componentDescriptor.FindTrait<ReflectionTraitHide>(false) != nullptr)
			{
				continue;
			}
			String name = GetComponentDisplayName(componentDescriptor);
			if (MatchSearch(name, search))
			{
				entries.PushBack({&componentDescriptor, name, selection->GetComponent(componentDescriptor) == nullptr});
			}
		}
		std::sort(entries.Begin(), entries.End(), [](const Entry& left, const Entry& right) { return left._name < right._name; });

		// The highlight: on the first result the search gives, moved by the arrows from one enabled result to the next (staying at the ends)
		int32_t count = static_cast<int32_t>(entries.Size());
		auto    findEnabled = [&](int32_t from, int32_t step) -> int32_t
		{
			for (int32_t index = from; index >= 0 && index < count; index += step)
			{
				if (entries[static_cast<uint32_t>(index)]._enabled)
				{
					return index;
				}
			}
			return -1;
		};
		if (searchEdited)
		{
			_addComponentHighlight = 0;
			_addComponentScrollToHighlight = true;
		}
		if (_addComponentMove != 0)
		{
			int32_t step = (_addComponentMove < 0) ? -1 : 1;
			for (int32_t moves = (_addComponentMove < 0) ? -_addComponentMove : _addComponentMove; moves > 0; --moves)
			{
				int32_t next = findEnabled(_addComponentHighlight + step, step);
				if (next == -1)
				{
					break;
				}
				_addComponentHighlight = next;
			}
			_addComponentScrollToHighlight = true;
		}
		if (_addComponentHighlight < 0 || _addComponentHighlight >= count || entries[static_cast<uint32_t>(_addComponentHighlight)]._enabled == false)
		{
			int32_t from = Clamp(_addComponentHighlight, 0, Max(count - 1, 0));
			_addComponentHighlight = findEnabled(from, 1);
			if (_addComponentHighlight == -1)
			{
				_addComponentHighlight = findEnabled(from, -1);
			}
		}

		const Entry* added = nullptr;
		if (enterPressed && _addComponentHighlight != -1)
		{
			added = &entries[static_cast<uint32_t>(_addComponentHighlight)];
		}

		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::SeparatorText("Components");
		ImGui::PopStyleVar();

		if (ImGui::BeginChild("ComponentList", ImVec2(0.0f, 350.0f), ImGuiChildFlags_FrameStyle))
		{
			if (entries.Empty())
			{
				ImGui::TextDisabled("No component matches \"%s\"", _addComponentSearch);
			}
			bool mouseMoved = (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f);
			for (int32_t index = 0; index < count; ++index)
			{
				const Entry& entry = entries[static_cast<uint32_t>(index)];
				ImGui::PushID(entry._descriptor);
				if (ImGui::Selectable(entry._name.CStr(), index == _addComponentHighlight, entry._enabled ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
				{
					added = &entry;
				}
				// The mouse moves the highlight too, so that the keyboard goes on from where it is
				if (entry._enabled && mouseMoved && ImGui::IsItemHovered())
				{
					_addComponentHighlight = index;
				}
				if (entry._enabled == false && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ImGui::SetTooltip("Already on this entity");
				}
				if (_addComponentScrollToHighlight && index == _addComponentHighlight)
				{
					ImGui::SetScrollHereY();
				}
				ImGui::PopID();
			}
			_addComponentScrollToHighlight = false;
		}
		ImGui::EndChild();

		if (added != nullptr)
		{
			selection->AddComponent(*added->_descriptor);
			GetOwner()->MarkAsDirty(fmt::format("Add {}", added->_name));
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	/// @brief
	/// @param object
	bool InspectorWindow::DrawDefaultInspector(EditorReflectedObject& reflectedObject)
	{
		return PropertyDrawer::DrawDescriptor(reflectedObject);
	}
}
