#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlChangesWindow.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlGui.hpp"

#include <HodEngine/Core/Math.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include <unordered_set>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(VersionControlChangesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	namespace
	{
		using Capability = VersionControlProvider::Capability;

		const ImU32 ErrorColor = IM_COL32(240, 80, 80, 255);
		const ImU32 WarningColor = IM_COL32(240, 180, 60, 255);

		bool IsSameOrUnder(const Path& path, const Path& parent)
		{
			const String& pathString = path.GetString();
			const String& parentString = parent.GetString();
			if (parentString.Empty() || pathString.StartsWith(parentString) == false)
			{
				return false;
			}
			return pathString.Size() == parentString.Size() || Path::IsSeparator(parentString.Back()) || Path::IsSeparator(pathString[parentString.Size()]);
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
	}

	/// @brief
	void VersionControlChangesWindow::Open()
	{
		VersionControlChangesWindow* window = static_cast<VersionControlChangesWindow*>(ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor()));
		if (window == nullptr)
		{
			ImGuiManager::GetInstance()->OpenWindow<VersionControlChangesWindow>()->CenterOnCurrentWindow();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	void VersionControlChangesWindow::OpenOnLastItem()
	{
		VersionControlChangesWindow* window = static_cast<VersionControlChangesWindow*>(ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor()));
		if (window == nullptr)
		{
			ImGuiManager::GetInstance()->OpenWindow<VersionControlChangesWindow>()->AnchorOnLastItem();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	/// @param itemMin
	/// @param itemMax
	/// @param viewport
	void VersionControlChangesWindow::OpenOnItem(const ImVec2& itemMin, const ImVec2& itemMax, ImGuiViewport* viewport)
	{
		VersionControlChangesWindow* window = static_cast<VersionControlChangesWindow*>(ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor()));
		if (window == nullptr)
		{
			ImGuiManager::GetInstance()->OpenWindow<VersionControlChangesWindow>()->AnchorOnItem(itemMin, itemMax, viewport);
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	/// @param only
	void VersionControlChangesWindow::Open(const Vector<Path>& only)
	{
		VersionControlChangesWindow* window = static_cast<VersionControlChangesWindow*>(ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor()));
		if (window == nullptr)
		{
			window = ImGuiManager::GetInstance()->OpenWindow<VersionControlChangesWindow>();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
		window->_only = only;
		window->_checked.clear();
	}

	/// @brief
	VersionControlChangesWindow::VersionControlChangesWindow()
	{
		SetTitle(ICON_MDI_SOURCE_COMMIT " Changes");
	}

	/// @brief
	void VersionControlChangesWindow::DrawContent()
	{
		VersionControl* versionControl = VersionControl::GetInstance();
		if (versionControl == nullptr || versionControl->IsConnected() == false)
		{
			const String error = versionControl != nullptr ? versionControl->GetConnectionError() : String();
			ImGui::TextDisabled("%s", error.Empty() ? "No version control connected" : error.CStr());
			return;
		}

		RefreshChanges();
		DrawHeader();
		ImGui::Separator();
		DrawChanges();
		DrawSubmit();
		VersionControlGui::DrawModals(); // Revert..., when no asset browser is open
	}

	/// @brief Again when the status cache changed. The checks of the files no longer changed forgotten
	void VersionControlChangesWindow::RefreshChanges()
	{
		VersionControl* versionControl = VersionControl::GetInstance();
		if (_changesLoaded && _statusVersion == versionControl->GetStatusVersion())
		{
			return;
		}
		_changesLoaded = true;
		_statusVersion = versionControl->GetStatusVersion();

		Vector<Change>           changes = versionControl->GetChanges();
		std::unordered_set<Path> originals;
		for (const Change& change : changes)
		{
			if (change._status._state == VersionControlState::Renamed && change._status._originalPath.Empty() == false)
			{
				originals.insert(change._status._originalPath);
			}
		}
		_changes.Clear();
		std::unordered_set<Path> present;
		for (Change& change : changes)
		{
			if (originals.contains(change._path) == false)
			{
				present.insert(change._path);
				_changes.PushBack(std::move(change));
			}
		}
		std::erase_if(_checked, [&present](const auto& entry) { return present.contains(entry.first) == false; });
	}

	/// @brief Provider, commits to push, refresh
	void VersionControlChangesWindow::DrawHeader()
	{
		VersionControl*               versionControl = VersionControl::GetInstance();
		const VersionControlProvider* provider = versionControl->GetConnectedProvider();
		const std::string_view        name = provider != nullptr ? provider->GetName() : std::string_view();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s %.*s", ICON_MDI_SOURCE_BRANCH, static_cast<int>(name.size()), name.data());

		if (versionControl->HasCapability(Capability::Push))
		{
			const uint32_t pending = versionControl->GetPendingPushCount();
			ImGui::SameLine();
			if (pending > 0)
			{
				ImGui::Text("- %u commit%s to push", pending, pending > 1 ? "s" : "");
			}
			else
			{
				ImGui::TextDisabled("- nothing to push");
			}
			ImGui::SameLine();
			ImGui::BeginDisabled(pending == 0 || _submit->_running);
			if (ImGui::Button(ICON_MDI_TRAY_ARROW_UP " Push"))
			{
				_submit->_running = true;
				_submit->_done = false;
				std::shared_ptr<SubmitState> submit = _submit;
				versionControl->Push(
					[submit](const VersionControlProvider::Result& result)
					{
						submit->_running = false;
						submit->_done = true;
						submit->_success = result._success;
						submit->_error = result._error;
					});
			}
			ImGui::EndDisabled();
		}

		const char* refreshLabel = ICON_MDI_REFRESH " Refresh";
		ImGui::SameLine();
		ImGui::SetCursorPosX(
			Max(ImGui::GetCursorPosX(), ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(refreshLabel).x - ImGui::GetStyle().FramePadding.x * 2.0f));
		if (ImGui::Button(refreshLabel))
		{
			versionControl->RequestRefresh();
		}
	}

	/// @brief One line per file: its check, its state, its path in the project
	void VersionControlChangesWindow::DrawChanges()
	{
		if (_submitHeight <= 0.0f) // First frame: estimate of the label, the message box and the buttons
		{
			_submitHeight = ImGui::GetTextLineHeightWithSpacing() * 7.0f + ImGui::GetFrameHeightWithSpacing() * 2.0f;
		}
		const float tableHeight = Max(ImGui::GetContentRegionAvail().y - _submitHeight, ImGui::GetFrameHeight() * 4.0f);

		if (_changes.Empty())
		{
			ImGui::BeginChild("NoChanges", ImVec2(0.0f, tableHeight));
			ImGui::TextDisabled("No change");
			ImGui::EndChild();
			return;
		}

		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable("Changes", 3, tableFlags, ImVec2(0.0f, tableHeight)) == false)
		{
			return;
		}
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("##Check", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetFrameHeight());
		ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("in conflict ").x + ImGui::GetFontSize() * 2.0f);

		// Header: the check of all of them
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		ImGui::TableSetColumnIndex(0);
		bool     allChecked = true;
		uint32_t submittable = 0;
		for (const Change& change : _changes)
		{
			if (IsSubmittable(change))
			{
				++submittable;
				allChecked &= IsChecked(change);
			}
		}
		allChecked &= submittable > 0;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Checkbox("##All", &allChecked))
		{
			for (const Change& change : _changes)
			{
				if (IsSubmittable(change))
				{
					SetChecked(change, allChecked);
				}
			}
		}
		ImGui::PopStyleVar();
		ImGui::TableSetColumnIndex(1);
		ImGui::TableHeader("File");
		ImGui::TableSetColumnIndex(2);
		ImGui::TableHeader("State");

		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(_changes.Size()));
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				const Change& change = _changes[static_cast<uint32_t>(row)];
				ImGui::PushID(row);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				const bool canSubmit = IsSubmittable(change);
				bool       checked = canSubmit && IsChecked(change);
				ImGui::BeginDisabled(canSubmit == false);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
				if (ImGui::Checkbox("##Check", &checked))
				{
					SetChecked(change, checked);
				}
				ImGui::PopStyleVar();
				ImGui::EndDisabled();
				if (canSubmit == false && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ImGui::SetTooltip("Resolve the conflict first (out of the editor)");
				}

				ImGui::TableSetColumnIndex(1);
				const VersionControlState state = change._status._state;
				String                    label = GetDisplayPath(change._path);
				if (state == VersionControlState::Renamed && change._status._originalPath.Empty() == false)
				{
					label = GetDisplayPath(change._status._originalPath) + "  " ICON_MDI_ARROW_RIGHT "  " + label;
				}
				const char* icon = VersionControlGui::GetStateIcon(state);
				ImGui::PushStyleColor(ImGuiCol_Text, VersionControlGui::GetStateColor(state));
				ImGui::TextUnformatted(icon != nullptr ? icon : " ");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				if (ImGui::Selectable(label.CStr(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick))
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (state == VersionControlState::Modified || state == VersionControlState::Renamed || state == VersionControlState::Conflicted)
						{
							VersionControl::GetInstance()->OpenDiff(change._path);
						}
					}
					else if (canSubmit)
					{
						SetChecked(change, IsChecked(change) == false);
					}
				}
				if (ImGui::BeginPopupContextItem("ChangeContext"))
				{
					DrawChangeContextMenu(change);
					ImGui::EndPopup();
				}
				if (change._status._lockedByMe)
				{
					ImGui::SameLine();
					ImGui::TextDisabled(ICON_MDI_LOCK);
				}

				ImGui::TableSetColumnIndex(2);
				ImGui::TextUnformatted(VersionControlGui::GetStateLabel(state));

				ImGui::PopID();
			}
		}
		ImGui::EndTable();
	}

	/// @brief
	/// @param change
	void VersionControlChangesWindow::DrawChangeContextMenu(const Change& change)
	{
		VersionControl*           versionControl = VersionControl::GetInstance();
		const VersionControlState state = change._status._state;

		const bool diffable = state == VersionControlState::Modified || state == VersionControlState::Renamed || state == VersionControlState::Conflicted;
		if (ImGui::MenuItem("Diff", nullptr, false, diffable && versionControl->HasCapability(Capability::Diff)))
		{
			versionControl->OpenDiff(change._path);
		}
		if (ImGui::MenuItem("History", nullptr, false, state != VersionControlState::Untracked && versionControl->HasCapability(Capability::History)))
		{
			versionControl->OpenHistory(change._path);
		}
		if (state == VersionControlState::Untracked && ImGui::MenuItem("Add"))
		{
			versionControl->Add({change._path});
		}
		if (state != VersionControlState::Untracked && ImGui::MenuItem("Revert..."))
		{
			Vector<Path> paths = {change._path};
			if (change._status._originalPath.Empty() == false)
			{
				paths.PushBack(change._status._originalPath);
			}
			VersionControlGui::RequestRevert(std::move(paths));
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Show in Explorer"))
		{
			OpenExplorerAtPath(state == VersionControlState::Deleted ? change._path.ParentPath() : change._path);
		}
		if (ImGui::MenuItem("Copy path"))
		{
			ImGui::SetClipboardText(change._path.GetString().CStr());
		}
	}

	/// @brief The message and the buttons
	void VersionControlChangesWindow::DrawSubmit()
	{
		const float startY = ImGui::GetCursorPosY();
		DrawSubmitContent();
		// Includes the trailing item spacing, which cancels the one after the table: the table ends exactly where the submit fits
		_submitHeight = ImGui::GetCursorPosY() - startY;
	}

	void VersionControlChangesWindow::DrawSubmitContent()
	{
		VersionControl* versionControl = VersionControl::GetInstance();

		uint32_t     checkedCount = 0;
		Vector<Path> paths;
		for (const Change& change : _changes)
		{
			if (IsSubmittable(change) && IsChecked(change))
			{
				++checkedCount;
				paths.PushBack(change._path);
			}
		}

		ImGui::Spacing();
		ImGui::Text("Message (%u file%s)", checkedCount, checkedCount > 1 ? "s" : "");
		ImGui::InputTextMultiline("##Message", _message, sizeof(_message), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 5.0f + ImGui::GetStyle().FramePadding.y * 2.0f));

		const bool hasMessage = std::string_view(_message).find_first_not_of(" \t\r\n") != std::string_view::npos;
		const bool canSubmit = versionControl->HasCapability(Capability::Submit) && checkedCount > 0 && hasMessage && _submit->_running == false;
		const bool canPush = versionControl->HasCapability(Capability::Push);

		auto submit = [this, &paths, versionControl](bool push)
		{
			_submit->_running = true;
			_submit->_done = false;
			std::shared_ptr<SubmitState> state = _submit;
			versionControl->Submit(std::move(paths), String(_message), push,
			                       [state](const VersionControlProvider::Result& result)
			                       {
									   state->_running = false;
									   state->_done = true;
									   state->_success = result._success;
									   state->_error = result._error;
								   });
		};

		ImGui::BeginDisabled(canSubmit == false);
		if (ImGui::Button(canPush ? ICON_MDI_SOURCE_COMMIT " Commit" : ICON_MDI_SOURCE_COMMIT " Submit"))
		{
			submit(false);
		}
		if (canPush)
		{
			ImGui::SameLine();
			if (ImGui::Button(ICON_MDI_TRAY_ARROW_UP " Commit & Push"))
			{
				submit(true);
			}
		}
		ImGui::EndDisabled();
		if (versionControl->HasCapability(Capability::Submit) == false)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("%s", versionControl->GetCapabilityHint(Capability::Submit).CStr());
		}
		else if (canSubmit == false && _submit->_running == false && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("%s", checkedCount == 0 ? "Check the files to submit" : "Write a message");
		}

		if (_submit->_running)
		{
			ImGui::SameLine();
			ImGui::TextDisabled(ICON_MDI_SYNC " Working...");
		}
		else if (_submit->_done)
		{
			if (_submit->_success)
			{
				_message[0] = '\0';
				_checked.clear();
				_only.Clear();
				_submit->_done = false;
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ErrorColor);
				ImGui::PushTextWrapPos(0.0f);
				ImGui::TextUnformatted(_submit->_error.CStr());
				ImGui::PopTextWrapPos();
				ImGui::PopStyleColor();
			}
		}

		if (const uint32_t dirtyTabs = CountDirtyTabs(); dirtyTabs > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, WarningColor);
			ImGui::Text(ICON_MDI_ALERT " %u tab%s with unsaved changes: not submitted until saved", dirtyTabs, dirtyTabs > 1 ? "s" : "");
			ImGui::PopStyleColor();
		}
	}

	/// @brief
	/// @param change
	/// @return
	bool VersionControlChangesWindow::IsChecked(const Change& change) const
	{
		auto it = _checked.find(change._path);
		if (it != _checked.end())
		{
			return it->second;
		}
		if (_only.Empty() == false)
		{
			for (const Path& path : _only)
			{
				if (IsSameOrUnder(change._path, path))
				{
					return true;
				}
			}
			return false;
		}
		return change._status._state != VersionControlState::Untracked; // To add knowingly
	}

	/// @brief
	/// @param change
	/// @return
	bool VersionControlChangesWindow::IsSubmittable(const Change& change) const
	{
		return change._status._state != VersionControlState::Conflicted;
	}

	/// @brief
	/// @param change
	/// @param checked
	void VersionControlChangesWindow::SetChecked(const Change& change, bool checked)
	{
		_checked[change._path] = checked;
	}

	/// @brief Relative to the project folder
	/// @param path
	/// @return
	String VersionControlChangesWindow::GetDisplayPath(const Path& path) const
	{
		const Path    projectDir = Project::GetInstance()->GetProjectPath().ParentPath();
		const String& full = path.GetString();
		const String& base = projectDir.GetString();
		if (base.Empty() == false && full.StartsWith(base) && full.Size() > base.Size())
		{
			const uint32_t start = Path::IsSeparator(base.Back()) ? base.Size() : base.Size() + 1;
			return full.SubStr(start);
		}
		return full;
	}
}
