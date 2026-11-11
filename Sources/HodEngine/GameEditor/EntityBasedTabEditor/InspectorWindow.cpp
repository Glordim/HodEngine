#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/InspectorWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/GameEditor/ComponentDisplayName.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

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
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"
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
				// What it is for, when its title stays hovered (its own description, not a parent's)
				ReflectionTraitDescription* description = component->GetReflectionDescriptorV().FindTrait<ReflectionTraitDescription>(false);
				bool opened = DrawHelper::BeginInspectorBlock((const void*)(uintptr_t)component->GetReflectionDescriptorV().GetType(), ICON_MDI_PUZZLE,
				                                              GetComponentDisplayName(component->GetReflectionDescriptorV()).CStr(), &enabled, &removeRequested, nullptr, nullptr,
				                                              nullptr, description != nullptr ? description->GetValue().CStr() : nullptr);
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

	/// @brief The list of the components that can be added. Without a search, browsed by folders (their categories, see
	/// ReflectionTraitCategory), those without one at the root after the folders; with a search, every component matching it by its name
	/// (first) or its category, whatever the folder, its folder shown. Driven with the keyboard too, the focus staying in the search field:
	/// the arrows move the highlighted row, Enter activates it (adds the component, enters the folder), and while the search is empty
	/// Right enters a folder, Left or Backspace go up one. Escape closes. The components the entity already has are greyed out (one of
	/// each type, see Entity::AddComponent), the description of each one is its tooltip.
	/// @param selection
	void InspectorWindow::DrawAddComponentPopup(Entity* selection)
	{
		// Called right after the Add Component button: the last item, the popup is placed by it
		ImRect button(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
		if (ImGui::BeginPopup("AddComponent") == false)
		{
			return;
		}

		// Always from the root
		if (ImGui::IsWindowAppearing())
		{
			_addComponentSearch[0] = '\0';
			_addComponentFolder.Clear();
			_addComponentHighlightFolder.Clear();
			_addComponentResetHighlight = true;
			_addComponentFocusSearch = true;
			_addComponentSlideDirection = 0;
		}

		bool searchWasEmpty = (_addComponentSearch[0] == '\0');
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
		ImGui::SameLine();
		if (_addComponentFocusSearch)
		{
			ImGui::SetKeyboardFocusHere();
			_addComponentFocusSearch = false;
		}
		_addComponentMove = 0;
		ImGui::SetNextItemWidth(-FLT_MIN);
		bool enterPressed = ImGui::InputTextWithHint("##Search", "Search", _addComponentSearch, sizeof(_addComponentSearch),
		                                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, &AddComponentSearchCallback,
		                                             &_addComponentMove);
		bool searchActive = ImGui::IsItemActive();
		if (ImGui::IsItemEdited())
		{
			_addComponentResetHighlight = true; // back on the folder browsed before when emptied
		}
		bool searching = (_addComponentSearch[0] != '\0');

		// The folder keys, in a search field that was and stays empty (not the Backspace erasing the last letter), not repeated
		bool goUp = false;
		bool goIn = false;
		if (searchActive && searchWasEmpty && searching == false)
		{
			goUp = ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false) || ImGui::IsKeyPressed(ImGuiKey_Backspace, false);
			goIn = ImGui::IsKeyPressed(ImGuiKey_RightArrow, false);
		}

		// The components that can be added (not the abstract ones, nor the hidden ones: bases not meant to be used alone, nor the
		// placeholder of an unknown one)
		struct Candidate
		{
			const ReflectionDescriptor* _descriptor;
			String                      _name;
			String                      _category;
			bool                        _enabled; // not on the entity yet
		};
		Vector<Candidate> candidates;
		for (const auto& componentDescriptorPair : ComponentFactory::GetInstance()->GetAllDescriptors())
		{
			const ReflectionDescriptor& componentDescriptor = *componentDescriptorPair.second;
			if (componentDescriptor.CanCreateInstance() == false || componentDescriptor.GetType() == MissingComponent::GetReflectionDescriptor().GetType() ||
			    componentDescriptor.FindTrait<ReflectionTraitHide>(false) != nullptr)
			{
				continue;
			}
			ReflectionTraitCategory* category = componentDescriptor.FindTrait<ReflectionTraitCategory>(); // inherited
			candidates.PushBack({&componentDescriptor, GetComponentDisplayName(componentDescriptor), category != nullptr ? category->GetValue() : String(),
			                     selection->GetComponent(componentDescriptor) == nullptr});
		}

		// The rows shown
		enum class RowType
		{
			Folder,
			Component,
		};
		struct Row
		{
			RowType                     _type;
			String                      _name;
			String                      _detail; // greyed on its right
			String                      _folder; // the one a Folder row leads to
			const ReflectionDescriptor* _descriptor = nullptr;
			bool                        _enabled = true;
			int32_t                     _rank = 0;
		};
		// The rows of a folder, without a search: the folders right inside it, then its components (the way back out of it is the header)
		auto buildFolderRows = [&](const String& folderPath) -> Vector<Row>
		{
			Vector<Row> folderRows;
			std::string_view folder(folderPath.CStr(), folderPath.Size());

			// The folders right inside this one, then its components
			Vector<String> subfolders;
			for (const Candidate& candidate : candidates)
			{
				std::string_view category(candidate._category.CStr(), candidate._category.Size());
				std::string_view rest;
				if (folder.empty())
				{
					rest = category;
				}
				else if (category.size() > folder.size() + 1 && category.substr(0, folder.size()) == folder && category[folder.size()] == '/')
				{
					rest = category.substr(folder.size() + 1);
				}
				if (rest.empty() == false)
				{
					String subfolder(rest.substr(0, rest.find('/')));
					if (std::find(subfolders.Begin(), subfolders.End(), subfolder) == subfolders.End())
					{
						subfolders.PushBack(subfolder);
					}
				}
			}
			std::sort(subfolders.Begin(), subfolders.End());
			for (const String& subfolder : subfolders)
			{
				folderRows.PushBack({RowType::Folder, subfolder, String(), folder.empty() ? subfolder : folderPath + "/" + subfolder});
			}

			Vector<Row> components;
			for (const Candidate& candidate : candidates)
			{
				if (candidate._category == folderPath)
				{
					components.PushBack({RowType::Component, candidate._name, String(), String(), candidate._descriptor, candidate._enabled});
				}
			}
			std::sort(components.Begin(), components.End(), [](const Row& left, const Row& right) { return left._name < right._name; });
			for (const Row& component : components)
			{
				folderRows.PushBack(component);
			}
			return folderRows;
		};

		Vector<Row> rows;
		if (searching)
		{
			// Matching by the name first, then by the category only
			String search(_addComponentSearch);
			for (const Candidate& candidate : candidates)
			{
				int32_t rank = MatchSearch(candidate._name, search) ? 0 : ((candidate._category.Empty() == false && MatchSearch(candidate._category, search)) ? 1 : -1);
				if (rank != -1)
				{
					rows.PushBack({RowType::Component, candidate._name, candidate._category, String(), candidate._descriptor, candidate._enabled, rank});
				}
			}
			std::sort(rows.Begin(), rows.End(),
			          [](const Row& left, const Row& right)
			          {
				          if (left._rank != right._rank)
				          {
					          return left._rank < right._rank;
				          }
				          return left._name < right._name;
			          });
		}
		else
		{
			rows = buildFolderRows(_addComponentFolder);
		}

		// The highlight: on the first row (not the way back) when the rows change, on the folder just left when going up, moved by
		// the arrows from one enabled row to the next (staying at the ends)
		int32_t count = static_cast<int32_t>(rows.Size());
		auto    findEnabled = [&](int32_t from, int32_t step) -> int32_t
		{
			for (int32_t index = from; index >= 0 && index < count; index += step)
			{
				if (rows[static_cast<uint32_t>(index)]._enabled)
				{
					return index;
				}
			}
			return -1;
		};
		if (_addComponentResetHighlight)
		{
			_addComponentHighlight = 0;
			_addComponentResetHighlight = false;
			_addComponentScrollToHighlight = true;
		}
		if (_addComponentHighlightFolder.Empty() == false)
		{
			for (int32_t index = 0; index < count; ++index)
			{
				if (rows[static_cast<uint32_t>(index)]._type == RowType::Folder && rows[static_cast<uint32_t>(index)]._folder == _addComponentHighlightFolder)
				{
					_addComponentHighlight = index;
				}
			}
			_addComponentHighlightFolder.Clear();
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
		if (_addComponentHighlight < 0 || _addComponentHighlight >= count || rows[static_cast<uint32_t>(_addComponentHighlight)]._enabled == false)
		{
			int32_t from = Clamp(_addComponentHighlight, 0, Max(count - 1, 0));
			_addComponentHighlight = findEnabled(from, 1);
			if (_addComponentHighlight == -1)
			{
				_addComponentHighlight = findEnabled(from, -1);
			}
		}

		const Row* activated = nullptr;
		if (_addComponentHighlight != -1)
		{
			const Row& highlighted = rows[static_cast<uint32_t>(_addComponentHighlight)];
			if (enterPressed || (goIn && highlighted._type == RowType::Folder))
			{
				activated = &highlighted;
			}
		}
		if (enterPressed)
		{
			_addComponentFocusSearch = true; // Enter leaves the field
		}

		// The folder browsed: its path from the root, each level above clickable to go back up to it ("Components" alone while searching:
		// the results come from every folder)
		bool   goToFolder = false;
		String goToFolderPath;
		{
			// "Components" then each level of the folder: the last one is where we are, the others links to go back up
			Vector<std::string_view> levels;
			levels.PushBack("Components");
			std::string_view folder(_addComponentFolder.CStr(), _addComponentFolder.Size());
			while (searching == false && folder.empty() == false)
			{
				size_t separator = folder.find('/');
				levels.PushBack(folder.substr(0, separator));
				folder = (separator == std::string_view::npos) ? std::string_view() : folder.substr(separator + 1);
			}
			for (uint32_t level = 0; level < levels.Size(); ++level)
			{
				String name(levels[level]);
				if (level > 0)
				{
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::TextDisabled(ICON_MDI_CHEVRON_RIGHT);
					ImGui::SameLine();
				}
				ImGui::AlignTextToFramePadding();
				if (level + 1 == levels.Size())
				{
					ImGui::TextUnformatted(name.CStr());
					continue;
				}
				ImGui::PushID(static_cast<int>(level));
				// A link: dimmer than the current level, but readable (TextDisabled isn't on the lighter background of the frames), lit up
				// when hovered (its rect known before drawing it: the text at the cursor, on the line's text baseline)
				ImVec2 linkMin = ImGui::GetCursorScreenPos();
				linkMin.y += ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset;
				ImVec2 linkMax(linkMin.x + ImGui::CalcTextSize(name.CStr()).x, linkMin.y + ImGui::GetTextLineHeight());
				bool   linkHovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(linkMin, linkMax);
				ImVec4 linkColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				if (linkHovered == false)
				{
					linkColor.w *= 0.6f;
				}
				ImGui::PushStyleColor(ImGuiCol_Text, linkColor);
				ImGui::TextUnformatted(name.CStr());
				ImGui::PopStyleColor();
				if (linkHovered)
				{
					// Underlined, the hand cursor
					ImVec2 min = ImGui::GetItemRectMin();
					ImVec2 max = ImGui::GetItemRectMax();
					ImGui::GetWindowDrawList()->AddLine(ImVec2(min.x, max.y), max, ImGui::GetColorU32(linkColor));
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						// The path of that level: its first levels joined
						goToFolder = true;
						goToFolderPath.Clear();
						for (uint32_t pathLevel = 1; pathLevel <= level; ++pathLevel)
						{
							if (pathLevel > 1)
							{
								goToFolderPath += "/";
							}
							goToFolderPath += levels[pathLevel];
						}
					}
				}
				ImGui::PopID();
			}
		}

		// While searching, each component's folder is on its right ("-" for none): the list as wide as the longest name and the longest
		// folder with a gap between them (the popup grows to fit it)
		constexpr const char* noFolder = "-";
		float                 listWidth = 0.0f;
		if (searching)
		{
			float nameWidth = 0.0f;
			float folderWidth = 0.0f;
			for (const Row& row : rows)
			{
				String label = String(DrawHelper::GetTypeIcon(*row._descriptor, ICON_MDI_PUZZLE)) + " " + row._name;
				nameWidth = Max(nameWidth, ImGui::CalcTextSize(label.CStr()).x);
				folderWidth = Max(folderWidth, ImGui::CalcTextSize(row._detail.Empty() ? noFolder : row._detail.CStr()).x);
			}
			const ImGuiStyle& style = ImGui::GetStyle();
			float             gap = style.ItemSpacing.x * 4.0f;
			listWidth = nameWidth + gap + folderWidth + style.FramePadding.x * 2.0f + style.ScrollbarSize;
			listWidth = Max(listWidth, ImGui::GetContentRegionAvail().x);
		}

		// Draws rows in the current child. Not interactive: the rows of the folder left, sliding out (drawn the same, only for the eye)
		auto drawRows = [&](const Vector<Row>& shownRows, int32_t highlight, bool interactive)
		{
			if (shownRows.Empty() && interactive)
			{
				if (searching)
				{
					ImGui::TextDisabled("No component matches \"%s\"", _addComponentSearch);
				}
				else
				{
					ImGui::TextDisabled("No component");
				}
			}
			bool mouseMoved = (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f);
			for (int32_t index = 0; index < static_cast<int32_t>(shownRows.Size()); ++index)
			{
				const Row& row = shownRows[static_cast<uint32_t>(index)];
				String     label;
				const char* right = nullptr;
				ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;
				if (row._type == RowType::Folder)
				{
					ImGui::PushID(row._folder.CStr());
					label = String(ICON_MDI_FOLDER " ") + row._name;
					right = ICON_MDI_CHEVRON_RIGHT;
					flags |= ImGuiSelectableFlags_NoAutoClosePopups;
				}
				else
				{
					ImGui::PushID(row._descriptor);
					label = String(DrawHelper::GetTypeIcon(*row._descriptor, ICON_MDI_PUZZLE)) + " " + row._name;
					if (searching)
					{
						right = row._detail.Empty() ? noFolder : row._detail.CStr();
					}
					if (row._enabled == false)
					{
						flags |= ImGuiSelectableFlags_Disabled;
					}
				}

				float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;
				if (ImGui::Selectable(label.CStr(), index == highlight, flags) && interactive)
				{
					activated = &row;
				}
				// The mouse moves the highlight too, so that the keyboard goes on from where it is
				if (interactive && row._enabled && mouseMoved && ImGui::IsItemHovered())
				{
					_addComponentHighlight = index;
				}
				if (interactive && row._type == RowType::Component && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ReflectionTraitDescription* description = row._descriptor->FindTrait<ReflectionTraitDescription>(false);
					if (row._enabled == false)
					{
						ImGui::SetTooltip("%s%sAlready on this entity", description != nullptr ? description->GetValue().CStr() : "", description != nullptr ? "\n\n" : "");
					}
					else if (description != nullptr)
					{
						ImGui::SetTooltip("%s", description->GetValue().CStr());
					}
				}
				if (interactive && _addComponentScrollToHighlight && index == highlight)
				{
					ImGui::SetScrollHereY();
				}
				if (right != nullptr)
				{
					ImGui::SameLine(rightEdge - ImGui::CalcTextSize(right).x);
					ImGui::TextDisabled("%s", right);
				}
				ImGui::PopID();
			}
		};

		// A folder change slides the rows: the folder left goes out on one side while the new one comes in from the other (from the right
		// into a folder, from the left back out of it), both in children of their own moving inside the frame, which stays
		if (searching)
		{
			_addComponentSlideDirection = 0;
		}
		if (ImGui::BeginChild("ComponentList", ImVec2(listWidth, 350.0f), ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImVec2 area = ImGui::GetContentRegionAvail();
			ImVec2 origin = ImGui::GetCursorPos();
			float  slideOffset = 0.0f;
			if (_addComponentSlideDirection != 0)
			{
				constexpr float duration = 0.25f;
				float           progress = static_cast<float>(ImGui::GetTime() - _addComponentSlideStart) / duration;
				if (progress >= 1.0f)
				{
					_addComponentSlideDirection = 0;
				}
				else
				{
					float remaining = 1.0f - progress;
					float eased = remaining * remaining * remaining; // ease out (cubic): fast, then settling
					slideOffset = static_cast<float>(_addComponentSlideDirection) * area.x * eased;
				}
			}
			if (_addComponentSlideDirection != 0)
			{
				ImGui::SetCursorPos(ImVec2(origin.x + slideOffset - static_cast<float>(_addComponentSlideDirection) * area.x, origin.y));
				if (ImGui::BeginChild("PreviousRows", area, ImGuiChildFlags_None, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar))
				{
					drawRows(buildFolderRows(_addComponentPreviousFolder), _addComponentPreviousHighlight, false);
				}
				ImGui::EndChild();
			}
			ImGui::SetCursorPos(ImVec2(origin.x + slideOffset, origin.y));
			if (ImGui::BeginChild("Rows", area))
			{
				drawRows(rows, _addComponentHighlight, true);
				_addComponentScrollToHighlight = false;
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		// A row activated (Enter, Right, click), or going back up (the header's arrow or one of its levels, Left, Backspace)
		int32_t previousHighlight = -1;
		if (activated != nullptr)
		{
			if (activated->_type == RowType::Component)
			{
				selection->AddComponent(*activated->_descriptor);
				GetOwner()->MarkAsDirty(fmt::format("Add {}", activated->_name));
				ImGui::CloseCurrentPopup();
			}
			else
			{
				goToFolder = true;
				goToFolderPath = activated->_folder;
				previousHighlight = static_cast<int32_t>(activated - &rows[0]);
			}
		}
		else if (goUp && searching == false && _addComponentFolder.Empty() == false)
		{
			std::string_view folder(_addComponentFolder.CStr(), _addComponentFolder.Size());
			size_t           separator = folder.rfind('/');
			goToFolder = true;
			goToFolderPath = (separator != std::string_view::npos) ? String(folder.substr(0, separator)) : String();
		}
		if (goToFolder && goToFolderPath != _addComponentFolder)
		{
			// Deeper: on its first row. Back up: on the folder the way came through, among its siblings
			std::string_view from(_addComponentFolder.CStr(), _addComponentFolder.Size());
			std::string_view to(goToFolderPath.CStr(), goToFolderPath.Size());
			bool             deeper = (from.empty() || (to.size() > from.size() && to.substr(0, from.size()) == from && to[from.size()] == '/'));
			if (deeper)
			{
				_addComponentResetHighlight = true;
			}
			else
			{
				std::string_view rest = to.empty() ? from : from.substr(to.size() + 1);
				std::string_view child = rest.substr(0, rest.find('/'));
				_addComponentHighlightFolder = to.empty() ? String(child) : goToFolderPath + "/" + child;
			}
			_addComponentPreviousFolder = _addComponentFolder;
			_addComponentPreviousHighlight = previousHighlight;
			_addComponentFolder = goToFolderPath;
			_addComponentFocusSearch = true; // a click took it
			_addComponentSlideDirection = deeper ? 1 : -1;
			_addComponentSlideStart = ImGui::GetTime();
		}

		// Centered on the button, right under it, or right above it when there's no room below; kept inside the area a popup may use (its
		// monitor's work area). Every frame, with its current size: it grows with a search's longer rows (ImGui only places a popup when
		// it opens), its size known from the second frame on (hidden on the first one, measured)
		ImGuiWindow* popupWindow = ImGui::GetCurrentWindow();
		ImRect       allowed = ImGui::GetPopupAllowedExtentRect(popupWindow);
		float        gap = ImGui::GetStyle().ItemInnerSpacing.y;
		ImVec2       position(button.GetCenter().x - popupWindow->Size.x * 0.5f, button.Max.y + gap);
		if (position.y + popupWindow->Size.y > allowed.Max.y && button.Min.y - gap - popupWindow->Size.y >= allowed.Min.y)
		{
			position.y = button.Min.y - gap - popupWindow->Size.y;
		}
		position.x = Max(Min(position.x, allowed.Max.x - popupWindow->Size.x), allowed.Min.x);
		position.y = Max(Min(position.y, allowed.Max.y - popupWindow->Size.y), allowed.Min.y);
		if (position.x != popupWindow->Pos.x || position.y != popupWindow->Pos.y)
		{
			ImGui::SetWindowPos(position);
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
