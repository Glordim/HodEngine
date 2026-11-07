#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlGui.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlChangesWindow.hpp"

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Vector.hpp>

#include <cctype>

namespace hod::inline editor
{
	namespace
	{
		using Capability = VersionControlProvider::Capability;

		constexpr const char* RevertModalName = "Revert changes?###VersionControlRevert";
		constexpr const char* PullModalName = "Pull with unsaved changes?###VersionControlPull";
		constexpr uint32_t    MaxListedFiles = 12;

		const ImU32 OutOfDateColor = IM_COL32(90, 190, 255, 255);
		const ImU32 LockedByMeColor = IM_COL32(110, 210, 110, 255);
		const ImU32 LockedByOtherColor = IM_COL32(240, 80, 80, 255);
		const ImU32 ErrorColor = IM_COL32(240, 80, 80, 255);
		const ImU32 BadgeBackgroundColor = IM_COL32(20, 20, 20, 200);

		Vector<Path> pendingRevert; // Waiting for its confirmation
		bool         pendingPull = false;
		int          modalsFrame = -1; // Drawn by the first asset browser of the frame only

		bool IsChanged(VersionControlState state)
		{
			return state == VersionControlState::Added || state == VersionControlState::Modified || state == VersionControlState::Deleted ||
			       state == VersionControlState::Renamed || state == VersionControlState::Conflicted;
		}

		/// @brief What a contextual menu action applies to: the selection (assets with all their files), the current folder on the void
		struct Targets
		{
			Vector<Path>                 _paths;    // To give to the operations
			Vector<VersionControlStatus> _statuses; // One per selected item
			Vector<const Asset*>         _assets;   // One per selected item, nullptr for a folder
			bool                         _hasFolder = false;

			template<typename Predicate>
			bool Any(Predicate predicate) const
			{
				for (const VersionControlStatus& status : _statuses)
				{
					if (predicate(status))
					{
						return true;
					}
				}
				return false;
			}
		};

		Targets GetTargets(const AssetBrowserWindow::Context& context)
		{
			Targets         targets;
			VersionControl* versionControl = VersionControl::GetInstance();
			Vector<Path>    items = context.selectedItems;
			if (items.Empty())
			{
				items.PushBack(context.currentDirectory);
			}
			for (const Path& item : items)
			{
				AssetDatabase::FileSystemMapping* mapping = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(item);
				if (mapping != nullptr && mapping->_asset != nullptr)
				{
					for (Path& file : VersionControl::GetAssetFiles(*mapping->_asset))
					{
						targets._paths.PushBack(std::move(file));
					}
					targets._statuses.PushBack(versionControl->GetAssetStatus(*mapping->_asset));
					targets._assets.PushBack(mapping->_asset.get());
				}
				else
				{
					targets._paths.PushBack(item);
					targets._statuses.PushBack(versionControl->GetFolderStatus(item));
					targets._assets.PushBack(nullptr);
					targets._hasFolder = true;
				}
			}
			return targets;
		}

		/// @brief The first file of the single asset selected that is changed (its .asset or one of its sources)
		Path GetChangedFile(const Targets& targets)
		{
			if (targets._assets.Size() != 1 || targets._assets[0] == nullptr)
			{
				return Path();
			}
			for (const Path& file : VersionControl::GetAssetFiles(*targets._assets[0]))
			{
				const VersionControlState state = VersionControl::GetInstance()->GetStatus(file)._state;
				if (state == VersionControlState::Modified || state == VersionControlState::Renamed || state == VersionControlState::Conflicted)
				{
					return file;
				}
			}
			return Path();
		}

		Vector<Path> GetProjectFolders()
		{
			const Project* project = Project::GetInstance();
			return {project->GetAssetDirPath(), project->GetSourceDirPath(), project->GetProjectSettingsDirPath()};
		}

		uint32_t CountDirtyTabs()
		{
			uint32_t count = 0;
			for (const EditorTab* tab : Editor::GetInstance()->GetEditorTabs())
			{
				count += tab->IsDirty() ? 1 : 0;
			}
			return count;
		}

		/// @brief With unsaved tabs, their saves would overwrite what the pull brings: confirmed first
		void RequestPull()
		{
			if (CountDirtyTabs() > 0)
			{
				pendingPull = true;
				return;
			}
			VersionControl::GetInstance()->UpdateWorkspace();
		}

		/// @brief The connection button's look
		struct ToolbarState
		{
			String   _label;
			ImU32    _color = 0; // 0: the style's
			String   _pullLabel;
			bool     _pull = false;
			uint32_t _outOfDate = 0;
			String   _changesLabel; // Empty: no Capability::Submit
			uint32_t _changeCount = 0;
			String   _pushLabel; // Empty: nothing to push
			uint32_t _pendingPush = 0;
		};

		/// @brief The changed files (untracked ones left out), counted again when the cache changes
		uint32_t GetChangeCount()
		{
			static uint32_t version = 0;
			static uint32_t count = 0;
			static bool     counted = false;
			VersionControl* versionControl = VersionControl::GetInstance();
			if (counted == false || version != versionControl->GetStatusVersion())
			{
				counted = true;
				version = versionControl->GetStatusVersion();
				count = 0;
				for (const VersionControl::Change& change : versionControl->GetChanges())
				{
					count += change._status._state != VersionControlState::Untracked ? 1 : 0;
				}
			}
			return count;
		}

		ToolbarState GetToolbarState()
		{
			ToolbarState    state;
			VersionControl* versionControl = VersionControl::GetInstance();
			if (versionControl == nullptr || versionControl->GetProviders().Empty())
			{
				return state;
			}

			const VersionControlProvider* provider = versionControl->GetConnectedProvider();
			const String                  error = versionControl->GetConnectionError();
			if (provider != nullptr)
			{
				state._label = String::Format("%s %.*s", versionControl->IsBusy() ? ICON_MDI_SYNC : ICON_MDI_SOURCE_BRANCH, static_cast<int>(provider->GetName().size()),
				                              provider->GetName().data());
			}
			else if (error.Empty() == false)
			{
				state._label = ICON_MDI_CLOUD_ALERT " Version control";
				state._color = ErrorColor;
			}
			else if (versionControl->IsBusy())
			{
				state._label = ICON_MDI_SYNC " Connecting";
			}
			else
			{
				state._label = ICON_MDI_CLOUD_OFF_OUTLINE;
				state._color = ImGui::GetColorU32(ImGuiCol_TextDisabled);
			}

			state._pull = versionControl->HasCapability(Capability::UpdateWorkspace);
			if (state._pull)
			{
				state._outOfDate = versionControl->GetOutOfDateCount();
				state._pullLabel = state._outOfDate > 0 ? String::Format(ICON_MDI_TRAY_ARROW_DOWN " Pull (%u)", state._outOfDate) : String(ICON_MDI_TRAY_ARROW_DOWN " Pull");
			}
			if (versionControl->HasCapability(Capability::Submit))
			{
				state._changeCount = GetChangeCount();
				state._changesLabel =
					state._changeCount > 0 ? String::Format(ICON_MDI_SOURCE_COMMIT " Changes (%u)", state._changeCount) : String(ICON_MDI_SOURCE_COMMIT " Changes");
			}
			state._pendingPush = versionControl->HasCapability(Capability::Push) ? versionControl->GetPendingPushCount() : 0;
			if (state._pendingPush > 0)
			{
				state._pushLabel = String::Format(ICON_MDI_TRAY_ARROW_UP " Push (%u)", state._pendingPush);
			}
			return state;
		}

		float CalcButtonWidth(const String& label)
		{
			return ImGui::CalcTextSize(label.CStr()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		}

		void DrawMenu()
		{
			VersionControl*               versionControl = VersionControl::GetInstance();
			const VersionControlProvider* provider = versionControl->GetConnectedProvider();
			const String                  error = versionControl->GetConnectionError();

			if (provider != nullptr)
			{
				ImGui::Text("%.*s: connected%s", static_cast<int>(provider->GetName().size()), provider->GetName().data(), versionControl->IsBusy() ? ", working..." : "");
			}
			else if (error.Empty() == false)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ErrorColor);
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
				ImGui::TextUnformatted(error.CStr());
				ImGui::PopTextWrapPos();
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::TextDisabled(versionControl->IsBusy() ? "Connecting..." : "No version control");
			}
			ImGui::Separator();

			if (ImGui::MenuItem(ICON_MDI_REFRESH " Refresh", nullptr, false, provider != nullptr))
			{
				versionControl->RequestRefresh();
			}
			if (versionControl->HasCapability(Capability::Submit) && ImGui::MenuItem(ICON_MDI_SOURCE_COMMIT " Changes..."))
			{
				VersionControlChangesWindow::Open();
			}
			if (versionControl->HasCapability(Capability::UpdateWorkspace) && ImGui::MenuItem(ICON_MDI_TRAY_ARROW_DOWN " Pull"))
			{
				RequestPull();
			}
			if (versionControl->HasCapability(Capability::Push) && ImGui::MenuItem(ICON_MDI_TRAY_ARROW_UP " Push", nullptr, false, versionControl->GetPendingPushCount() > 0))
			{
				versionControl->Push();
			}
			if ((versionControl->HasCapability(Capability::Checkout) || versionControl->HasCapability(Capability::Lock)) &&
			    ImGui::MenuItem("Revert Unchanged", nullptr, false, true))
			{
				versionControl->RevertUnchanged(GetProjectFolders());
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("Releases the checkouts and locks of the unchanged files of the project");
			}
			ImGui::Separator();

			if (ImGui::BeginMenu("Provider"))
			{
				const std::string_view choice = versionControl->GetProviderChoice();
				if (ImGui::MenuItem("Auto", nullptr, choice == "Auto"))
				{
					versionControl->SetProviderChoice("Auto");
				}
				if (ImGui::MenuItem("None", nullptr, choice == "None"))
				{
					versionControl->SetProviderChoice("None");
				}
				ImGui::Separator();
				for (const VersionControlProvider* registered : versionControl->GetProviders())
				{
					const String name(registered->GetName());
					if (ImGui::MenuItem(name.CStr(), nullptr, choice == registered->GetName()))
					{
						versionControl->SetProviderChoice(registered->GetName());
					}
				}
				ImGui::EndMenu();
			}
			bool autoAdd = versionControl->GetAutoAdd();
			if (ImGui::MenuItem("Add new files automatically", nullptr, &autoAdd))
			{
				versionControl->SetAutoAdd(autoAdd);
			}
			bool checkoutOnEdit = versionControl->GetCheckoutOnEdit();
			if (ImGui::MenuItem("Checkout on first edit", nullptr, &checkoutOnEdit))
			{
				versionControl->SetCheckoutOnEdit(checkoutOnEdit);
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("Checks out (locks) the files of an asset on its first change in a tab, rather than on its save");
			}
		}

		void DrawPathList(const Vector<Path>& paths)
		{
			for (uint32_t index = 0; index < paths.Size() && index < MaxListedFiles; ++index)
			{
				ImGui::BulletText("%s", paths[index].GetString().CStr());
			}
			if (paths.Size() > MaxListedFiles)
			{
				ImGui::TextDisabled("and %u more", paths.Size() - MaxListedFiles);
			}
		}
	}

	/// @brief
	/// @param status
	/// @return
	bool VersionControlGui::HasBadge(const VersionControlStatus& status)
	{
		return GetStateIcon(status._state) != nullptr || status._outOfDate || status._lockedByMe || status._lockedByOther;
	}

	/// @brief
	/// @param drawList
	/// @param min
	/// @param max
	/// @param status
	void VersionControlGui::DrawBadges(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, const VersionControlStatus& status)
	{
		const float size = ImGui::GetFontSize();
		const float radius = size * 0.65f;

		auto drawBadge = [drawList, size, radius](const ImVec2& center, const char* icon, ImU32 color)
		{
			drawList->AddCircleFilled(center, radius, BadgeBackgroundColor);
			const ImVec2 iconSize = ImGui::CalcTextSize(icon);
			drawList->AddText(ImGui::GetFont(), size, ImVec2(center.x - iconSize.x * 0.5f, center.y - iconSize.y * 0.5f), color, icon);
		};

		if (const char* icon = GetStateIcon(status._state); icon != nullptr)
		{
			drawBadge(ImVec2(min.x + radius, max.y - radius), icon, GetStateColor(status._state));
		}
		if (status._outOfDate)
		{
			drawBadge(ImVec2(min.x + radius, min.y + radius), ICON_MDI_ARROW_DOWN_CIRCLE, OutOfDateColor);
		}
		if (status._lockedByOther || status._lockedByMe)
		{
			drawBadge(ImVec2(max.x - radius, min.y + radius), ICON_MDI_LOCK, status._lockedByOther ? LockedByOtherColor : LockedByMeColor);
		}
	}

	/// @brief
	/// @param status
	void VersionControlGui::DrawInlineBadges(const VersionControlStatus& status)
	{
		auto drawIcon = [](const char* icon, ImU32 color)
		{
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(icon);
			ImGui::PopStyleColor();
		};

		if (const char* icon = GetStateIcon(status._state); icon != nullptr)
		{
			drawIcon(icon, GetStateColor(status._state));
		}
		if (status._outOfDate)
		{
			drawIcon(ICON_MDI_ARROW_DOWN_CIRCLE, OutOfDateColor);
		}
		if (status._lockedByOther || status._lockedByMe)
		{
			drawIcon(ICON_MDI_LOCK, status._lockedByOther ? LockedByOtherColor : LockedByMeColor);
		}
	}

	/// @brief
	/// @param status
	/// @param folder
	void VersionControlGui::DrawTooltip(const VersionControlStatus& status, bool folder)
	{
		if (HasBadge(status) == false || ImGui::BeginTooltip() == false)
		{
			return;
		}

		if (const char* icon = GetStateIcon(status._state); icon != nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, GetStateColor(status._state));
			ImGui::TextUnformatted(icon);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			if (folder)
			{
				ImGui::Text("Contains files %s", GetStateLabel(status._state));
			}
			else
			{
				const char* label = GetStateLabel(status._state);
				ImGui::Text("%c%s", static_cast<char>(std::toupper(static_cast<unsigned char>(label[0]))), label + 1);
			}
		}
		if (status._outOfDate)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, OutOfDateColor);
			ImGui::TextUnformatted(ICON_MDI_ARROW_DOWN_CIRCLE);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::TextUnformatted(folder ? "Contains files changed on the server (Pull)" : "Changed on the server (Pull)");
		}
		if (status._lockedByMe)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, LockedByMeColor);
			ImGui::TextUnformatted(ICON_MDI_LOCK);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::TextUnformatted(folder ? "Contains files locked by you" : "Locked by you");
		}
		if (status._lockedByOther)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, LockedByOtherColor);
			ImGui::TextUnformatted(ICON_MDI_LOCK);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::Text("%s %s", folder ? "Contains files locked by" : "Locked by", status._otherUsers.Empty() ? "someone else" : status._otherUsers.CStr());
		}
		ImGui::EndTooltip();
	}

	/// @brief
	/// @return
	float VersionControlGui::CalcToolbarWidth()
	{
		const ToolbarState state = GetToolbarState();
		if (state._label.Empty())
		{
			return 0.0f;
		}
		float width = CalcButtonWidth(state._label);
		for (const String* label : {&state._changesLabel, &state._pullLabel, &state._pushLabel})
		{
			if (label->Empty() == false)
			{
				width += ImGui::GetStyle().ItemSpacing.x + CalcButtonWidth(*label);
			}
		}
		return width;
	}

	/// @brief
	void VersionControlGui::DrawToolbar()
	{
		const ToolbarState state = GetToolbarState();
		if (state._label.Empty())
		{
			return;
		}

		if (state._color != 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, state._color);
		}
		const bool clicked = ImGui::Button((state._label + "###VersionControlButton").CStr());
		if (state._color != 0)
		{
			ImGui::PopStyleColor();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			const String error = VersionControl::GetInstance()->GetConnectionError();
			ImGui::SetTooltip("%s", error.Empty() ? "Version control" : error.CStr());
		}
		if (clicked)
		{
			ImGui::OpenPopup("VersionControlMenu");
		}
		if (ImGui::BeginPopup("VersionControlMenu"))
		{
			DrawMenu();
			ImGui::EndPopup();
		}

		if (state._changesLabel.Empty() == false)
		{
			ImGui::SameLine();
			if (ImGui::Button((state._changesLabel + "###VersionControlChanges").CStr()))
			{
				VersionControlChangesWindow::Open();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("The changes of the project, to submit");
			}
		}

		if (state._pull)
		{
			ImGui::SameLine();
			if (state._outOfDate > 0)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, OutOfDateColor);
			}
			const bool pull = ImGui::Button((state._pullLabel + "###VersionControlPull").CStr());
			if (state._outOfDate > 0)
			{
				ImGui::PopStyleColor();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				if (state._outOfDate > 0)
				{
					ImGui::SetTooltip("%u files of the project changed on the server (as of the last fetch)", state._outOfDate);
				}
				else
				{
					ImGui::SetTooltip("Up to date as of the last fetch");
				}
			}
			if (pull)
			{
				RequestPull();
			}
		}

		if (state._pushLabel.Empty() == false)
		{
			ImGui::SameLine();
			if (ImGui::Button((state._pushLabel + "###VersionControlPush").CStr()))
			{
				VersionControl::GetInstance()->Push();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("%u local commit%s to send to the server", state._pendingPush, state._pendingPush > 1 ? "s" : "");
			}
		}
	}

	/// @brief
	/// @param paths
	void VersionControlGui::RequestRevert(Vector<Path> paths)
	{
		pendingRevert = std::move(paths);
	}

	/// @brief
	void VersionControlGui::DrawModals()
	{
		if (modalsFrame == ImGui::GetFrameCount())
		{
			return;
		}
		modalsFrame = ImGui::GetFrameCount();

		VersionControl* versionControl = VersionControl::GetInstance();
		const ImVec2    center = ImGui::GetMainViewport()->GetCenter();

		if (pendingRevert.Empty() == false && ImGui::IsPopupOpen(RevertModalName) == false)
		{
			ImGui::OpenPopup(RevertModalName);
		}
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(RevertModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextUnformatted("The local changes of these files (and of what's under these folders) will be lost:");
			DrawPathList(pendingRevert);
			ImGui::Spacing();
			if (ImGui::Button("Revert"))
			{
				versionControl->Revert(std::move(pendingRevert));
				pendingRevert.Clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				pendingRevert.Clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (pendingPull && ImGui::IsPopupOpen(PullModalName) == false)
		{
			ImGui::OpenPopup(PullModalName);
		}
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(PullModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%u tabs have unsaved changes.", CountDirtyTabs());
			ImGui::TextUnformatted("Saving them after the pull would overwrite what it brings to their files.");
			ImGui::Spacing();
			if (ImGui::Button("Pull"))
			{
				versionControl->UpdateWorkspace();
				pendingPull = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				pendingPull = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	/// @brief Shown as soon as a provider is connected: disabled with the reason when the provider can't (its capability hint) or
	/// when the selection has nothing for it
	void VersionControlGui::RegisterContextualMenuActions()
	{
		using Context = AssetBrowserWindow::Context;
		using Predicate = bool (*)(const VersionControlStatus& status);

		auto connected = [](const Context&) { return VersionControl::GetInstance() != nullptr && VersionControl::GetInstance()->IsConnected(); };

		// The capability's hint, else whyNot if no selected item matches (a folder always does: what's under it isn't known here)
		auto reason = [](uint32_t capability, Predicate predicate, const char* whyNot)
		{
			return [capability, predicate, whyNot](const Context& context) -> String
			{
				if (capability != Capability::None)
				{
					String hint = VersionControl::GetInstance()->GetCapabilityHint(capability);
					if (hint.Empty() == false)
					{
						return hint;
					}
				}
				if (predicate != nullptr)
				{
					const Targets targets = GetTargets(context);
					if (targets._hasFolder == false && targets.Any(predicate) == false)
					{
						return whyNot;
					}
				}
				return String();
			};
		};

		auto add = [&connected](const char* path, std::function<String(const Context&)> disabledReason, std::function<void(const Context&)> execute)
		{
			AssetBrowserWindow::RegisterContextualMenuAction({
				.path = path,
				.group = "Version Control",
				.available = connected,
				.execute = std::move(execute),
				.disabledReason = std::move(disabledReason),
			});
		};

		add("Version Control/Submit...",
		    reason(
				Capability::Submit, [](const VersionControlStatus& status) { return IsChanged(status._state) || status._state == VersionControlState::Untracked; },
				"Nothing changed in the selection"),
		    [](const Context& context) { VersionControlChangesWindow::Open(GetTargets(context)._paths); });
		add("Version Control/Refresh", nullptr, [](const Context& context) { VersionControl::GetInstance()->RequestRefresh(GetTargets(context)._paths); });
		add("Version Control/Add",
		    reason(
				Capability::None, [](const VersionControlStatus& status) { return status._state == VersionControlState::Untracked; }, "Nothing to add in the selection"),
		    [](const Context& context) { VersionControl::GetInstance()->Add(GetTargets(context)._paths); });
		add("Version Control/Checkout",
		    reason(
				Capability::Checkout,
				[](const VersionControlStatus& status) { return status._state == VersionControlState::Unchanged && status._lockedByMe == false && status._lockedByOther == false; },
				"Nothing to check out in the selection (already checked out, changed or locked)"),
		    [](const Context& context) { VersionControl::GetInstance()->Checkout(GetTargets(context)._paths); });
		add("Version Control/Lock",
		    reason(
				Capability::Lock,
				[](const VersionControlStatus& status)
				{
					return status._lockedByMe == false && status._lockedByOther == false && status._state != VersionControlState::Untracked &&
					       status._state != VersionControlState::Added;
				},
				"Nothing to lock in the selection (already locked, or not committed yet)"),
		    [](const Context& context) { VersionControl::GetInstance()->Lock(GetTargets(context)._paths); });
		add("Version Control/Unlock",
		    reason(
				Capability::Lock, [](const VersionControlStatus& status) { return status._lockedByMe; }, "Nothing locked by you in the selection"),
		    [](const Context& context) { VersionControl::GetInstance()->Unlock(GetTargets(context)._paths); });
		add("Version Control/Get Latest",
		    reason(
				Capability::GetLatestFiles, [](const VersionControlStatus& status) { return status._outOfDate; }, "Nothing changed on the server in the selection"),
		    [](const Context& context) { VersionControl::GetInstance()->GetLatest(GetTargets(context)._paths); });
		add("Version Control/Revert...",
		    reason(
				Capability::None, [](const VersionControlStatus& status) { return IsChanged(status._state) || status._lockedByMe; }, "Nothing to revert in the selection"),
		    [](const Context& context) { pendingRevert = GetTargets(context)._paths; });
		add(
			"Version Control/Revert Unchanged",
			[](const Context& context) -> String
			{
				VersionControl* versionControl = VersionControl::GetInstance();
				if (versionControl->HasCapability(Capability::Checkout) == false && versionControl->HasCapability(Capability::Lock) == false)
				{
					return versionControl->GetCapabilityHint(Capability::Lock);
				}
				const Targets targets = GetTargets(context);
				if (targets._hasFolder == false &&
				    targets.Any([](const VersionControlStatus& status) { return status._lockedByMe && status._state == VersionControlState::Unchanged; }) == false)
				{
					return "Nothing locked by you and unchanged in the selection";
				}
				return String();
			},
			[](const Context& context) { VersionControl::GetInstance()->RevertUnchanged(GetTargets(context)._paths); });
		add(
			"Version Control/Diff",
			[](const Context& context) -> String
			{
				String hint = VersionControl::GetInstance()->GetCapabilityHint(Capability::Diff);
				if (hint.Empty() == false)
				{
					return hint;
				}
				return GetChangedFile(GetTargets(context)).Empty() ? String("Only for one modified asset") : String();
			},
			[](const Context& context)
			{
				const Path file = GetChangedFile(GetTargets(context));
				if (file.Empty() == false)
				{
					VersionControl::GetInstance()->OpenDiff(file);
				}
			});
		add(
			"Version Control/History",
			[](const Context& context) -> String
			{
				String hint = VersionControl::GetInstance()->GetCapabilityHint(Capability::History);
				if (hint.Empty() == false)
				{
					return hint;
				}
				return context.selectedItems.Size() > 1 ? String("Only for one item") : String();
			},
			[](const Context& context)
			{
				const Path path = context.selectedItems.Empty() ? context.currentDirectory : context.selectedItems[0];
				VersionControl::GetInstance()->OpenHistory(path);
			});
	}

	/// @brief Nothing for the states not worth a badge
	/// @param state
	/// @return
	const char* VersionControlGui::GetStateIcon(VersionControlState state)
	{
		switch (state)
		{
			case VersionControlState::Untracked: return ICON_MDI_HELP_CIRCLE;
			case VersionControlState::Added: return ICON_MDI_PLUS_CIRCLE;
			case VersionControlState::Modified: return ICON_MDI_PENCIL_CIRCLE;
			case VersionControlState::Deleted: return ICON_MDI_MINUS_CIRCLE;
			case VersionControlState::Renamed: return ICON_MDI_ARROW_RIGHT_CIRCLE;
			case VersionControlState::Conflicted: return ICON_MDI_ALERT_CIRCLE;
			case VersionControlState::Unknown:
			case VersionControlState::Ignored:
			case VersionControlState::Unchanged: return nullptr;
		}
		return nullptr;
	}

	/// @brief
	/// @param state
	/// @return
	ImU32 VersionControlGui::GetStateColor(VersionControlState state)
	{
		switch (state)
		{
			case VersionControlState::Untracked: return IM_COL32(170, 170, 170, 255);
			case VersionControlState::Added: return IM_COL32(110, 210, 110, 255);
			case VersionControlState::Modified: return IM_COL32(240, 180, 60, 255);
			case VersionControlState::Deleted: return IM_COL32(240, 80, 80, 255);
			case VersionControlState::Renamed: return IM_COL32(170, 140, 255, 255);
			case VersionControlState::Conflicted: return IM_COL32(255, 60, 60, 255);
			case VersionControlState::Unknown:
			case VersionControlState::Ignored:
			case VersionControlState::Unchanged: break;
		}
		return ImGui::GetColorU32(ImGuiCol_TextDisabled);
	}

	/// @brief Completes "Contains files ..." too
	/// @param state
	/// @return
	const char* VersionControlGui::GetStateLabel(VersionControlState state)
	{
		switch (state)
		{
			case VersionControlState::Unknown: return "unknown";
			case VersionControlState::Ignored: return "ignored";
			case VersionControlState::Untracked: return "not added";
			case VersionControlState::Unchanged: return "unchanged";
			case VersionControlState::Added: return "added";
			case VersionControlState::Modified: return "modified";
			case VersionControlState::Deleted: return "deleted";
			case VersionControlState::Renamed: return "renamed";
			case VersionControlState::Conflicted: return "in conflict";
		}
		return "";
	}
}
