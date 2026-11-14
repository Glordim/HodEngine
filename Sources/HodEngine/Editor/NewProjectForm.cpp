#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/NewProjectForm.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Core/FileSystem/FileSystem.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <portable-file-dialogs.h>

#include <cstring>

namespace hod::inline editor
{
	namespace
	{
		const ImVec4 ErrorColor(1.0f, 0.4f, 0.4f, 1.0f);
		const ImVec4 WarningColor(1.0f, 0.6f, 0.2f, 1.0f);

		/// @brief Only what Project::CheckName accepts (a leading digit is told by the check)
		int FilterNameCharacter(ImGuiInputTextCallbackData* data)
		{
			ImWchar c = data->EventChar;
			bool    valid = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
			return valid ? 0 : 1;
		}

		void CopyTo(char* buffer, size_t size, const String& value)
		{
			size_t length = Min(static_cast<size_t>(value.Size()), size - 1);
			std::memcpy(buffer, value.CStr(), length);
			buffer[length] = '\0';
		}

		bool Contains(const Vector<String>& names, const String& name)
		{
			for (const String& other : names)
			{
				if (other == name)
				{
					return true;
				}
			}
			return false;
		}
	}

	/// @brief
	/// @param location
	void NewProjectForm::Reset(const Path& location)
	{
		_name[0] = '\0';
		CopyTo(_location, sizeof(_location), location.GetString());
		_focusName = true;

		_pluginGraph = PluginGraph();
		Project::BuildEngineModuleGraph(_pluginGraph);
		_disabledPlugins.Clear();
		_resetModulesHeader = true;
		_contentHeight = 0.0f; // Known once drawn
		_versionControl.Clear();
		_creationError.Clear();
	}

	/// @brief
	/// @return
	Path NewProjectForm::GetProjectDirectory() const
	{
		if (_location[0] == '\0' || _name[0] == '\0')
		{
			return Path();
		}
		return (Path(_location) / _name).LexicallyNormal();
	}

	/// @brief
	/// @return
	NewProjectForm::Result NewProjectForm::Draw()
	{
		_pluginGraph.Resolve(_disabledPlugins);

		// The version control systems of the enabled plugins, the choice dropped if its plugin was disabled
		_versionControlSystems.Clear();
		for (const PluginGraph::Plugin& plugin : _pluginGraph.GetPlugins())
		{
			if (plugin._enabled)
			{
				for (const String& system : plugin._manifest._versionControlSystems)
				{
					_versionControlSystems.PushBack(system);
				}
			}
		}
		if (_versionControl.Empty() == false && Contains(_versionControlSystems, _versionControl) == false)
		{
			_versionControl.Clear();
		}

		const float labelWidth = ImGui::CalcTextSize("Version control").x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
		auto        label = [labelWidth](const char* text)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(text);
			ImGui::SameLine(labelWidth);
		};

		ImGui::TextUnformatted("New project");
		ImGui::Separator();

		label("Name");
		ImGui::SetNextItemWidth(-1.0f);
		if (_focusName)
		{
			ImGui::SetKeyboardFocusHere();
			_focusName = false;
		}
		ImGui::InputTextWithHint("##Name", "MyGame", _name, sizeof(_name), ImGuiInputTextFlags_CallbackCharFilter, &FilterNameCharacter);

		label("Location");
		const char* browseLabel = ICON_MDI_FOLDER_OPEN " Browse...";
		ImGui::SetNextItemWidth(-(ImGui::CalcTextSize(browseLabel).x + ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x));
		ImGui::InputTextWithHint("##Location", "The folder the project folder is created in", _location, sizeof(_location));
		ImGui::SameLine();
		if (ImGui::Button(browseLabel))
		{
			pfd::select_folder dialog("Where to create the project", _location);
			std::string        result = dialog.result();
			if (result.empty() == false)
			{
				CopyTo(_location, sizeof(_location), String(result.c_str()));
			}
		}

		// What the two give, and whether it can be created
		const Path projectDirectory = GetProjectDirectory();
		String     error;
		if (_location[0] == '\0')
		{
			error = "Choose where to create the project";
		}
		else if (_name[0] == '\0')
		{
			error = "Name the project";
		}
		else
		{
			error = Project::CheckNewProjectDirectory(projectDirectory);
		}

		ImGui::SetCursorPosX(labelWidth); // Under the fields
		if (projectDirectory.Empty() == false)
		{
			ImGui::TextDisabled("%s", (projectDirectory / (String(_name) + ".hod")).GetString().CStr());
		}
		else
		{
			ImGui::TextDisabled("<location>%c<name>%c<name>.hod", Path::PreferredSeparator, Path::PreferredSeparator);
		}
		ImGui::SetCursorPosX(labelWidth);
		if (error.Empty() == false)
		{
			ImGui::TextColored(ErrorColor, ICON_MDI_ALERT " %s", error.CStr());
		}
		else if (FileSystem::GetInstance()->Exists(Path(_location)) == false)
		{
			ImGui::TextDisabled("The location doesn't exist yet: it is created");
		}
		else
		{
			ImGui::TextDisabled("Everything of the project goes in its own folder, %s", _name);
		}

		label("Version control");
		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::BeginCombo("##VersionControl", _versionControl.Empty() ? "None" : _versionControl.CStr()))
		{
			if (ImGui::Selectable("None", _versionControl.Empty()))
			{
				_versionControl.Clear();
			}
			for (const String& system : _versionControlSystems)
			{
				if (ImGui::Selectable(system.CStr(), _versionControl == system))
				{
					_versionControl = system;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (_versionControl.Empty())
		{
			ImGui::TextDisabled("A version control the location is already under is used all the same");
		}
		else
		{
			ImGui::TextDisabled("Its workspace is created in the project folder, unless it's under one already");
		}

		ImGui::Spacing();
		DrawModules();

		// Footer
		if (_creationError.Empty() == false)
		{
			ImGui::TextColored(ErrorColor, ICON_MDI_ALERT " %s", _creationError.CStr());
			ImGui::SameLine();
		}
		const char* createLabel = "Create";
		const char* cancelLabel = "Cancel";
		float       buttonsWidth = ImGui::CalcTextSize(createLabel).x + ImGui::CalcTextSize(cancelLabel).x + ImGui::GetStyle().FramePadding.x * 4.0f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Max(0.0f, ImGui::GetContentRegionAvail().x - buttonsWidth));
		if (ImGui::Button(cancelLabel))
		{
			return Result::Cancelled;
		}
		ImGui::SameLine();
		ImGui::BeginDisabled(error.Empty() == false);
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 0, 255));
		bool create = ImGui::Button(createLabel);
		ImGui::PopStyleColor();
		ImGui::EndDisabled();
		_contentHeight = ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y + ImGui::GetStyle().WindowPadding.y;
		if (create)
		{
			Project::CreateInfo createInfo;
			createInfo._directory = projectDirectory;
			createInfo._disabledPlugins = _disabledPlugins;
			if (Editor::GetInstance()->CreateProject(createInfo, _versionControl) == false)
			{
				_creationError = "The project couldn't be created or opened (see the console)";
				return Result::None;
			}
			return Result::Created;
		}
		return Result::None;
	}

	/// @brief
	/// @return
	float NewProjectForm::GetContentHeight() const
	{
		return _contentHeight;
	}

	/// @brief The engine plugins, as Edit > Plugins (changed there afterwards). Collapsed by default, what's disabled told in its title
	void NewProjectForm::DrawModules()
	{
		uint32_t disabledCount = 0;
		for (const PluginGraph::Plugin& plugin : _pluginGraph.GetPlugins())
		{
			disabledCount += plugin._enabled ? 0 : 1;
		}
		String title = disabledCount == 0 ? String("Plugins: all enabled") : String::Format("Plugins: %u disabled", disabledCount);
		title += "###Plugins";

		if (_resetModulesHeader)
		{
			ImGui::SetNextItemOpen(false);
			_resetModulesHeader = false;
		}
		bool shown = ImGui::CollapsingHeader(title.CStr());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("The engine plugins the project uses, all by default. Changed afterwards in Edit > Plugins");
		}
		if (shown == false)
		{
			return;
		}

		// As high as its rows (the window fits the form, see GetContentHeight), up to MaxVisibleRows: a scrollbar then
		constexpr uint32_t    MaxVisibleRows = 8;
		const float           rowHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().CellPadding.y * 2.0f;
		const uint32_t        visibleRows = Min(static_cast<uint32_t>(_pluginGraph.GetPlugins().Size()), MaxVisibleRows);
		const float           tableHeight = rowHeight * static_cast<float>(visibleRows + 1); // + the headers
		const ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("Plugins", 3, tableFlags, ImVec2(0.0f, tableHeight)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("##Enabled", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFrameHeight());
			ImGui::TableSetupColumn("Plugin", ImGuiTableColumnFlags_WidthStretch, 2.0f);
			ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableHeadersRow();

			for (const PluginGraph::Plugin& plugin : _pluginGraph.GetPlugins())
			{
				const PluginManifest& manifest = plugin._manifest;
				ImGui::PushID(manifest._name.CStr());
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				bool asked = Contains(_disabledPlugins, manifest._name) == false;
				ImGui::BeginDisabled(manifest._required);
				if (ImGui::Checkbox("##Enabled", &asked))
				{
					_pluginGraph.SetEnabled(_disabledPlugins, manifest._name, asked);
				}
				ImGui::EndDisabled();

				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(manifest._name.CStr());
				if (manifest._description.Empty() == false)
				{
					ImGui::SameLine();
					ImGui::TextDisabled("%s", manifest._description.CStr());
				}

				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				if (plugin._enabled == false && asked)
				{
					ImGui::TextColored(WarningColor, ICON_MDI_ALERT " %s", plugin._disabledReason.CStr());
				}
				else if (plugin._enabled == false)
				{
					ImGui::TextDisabled("%s", plugin._disabledReason.CStr());
				}
				else if (manifest._required)
				{
					ImGui::TextDisabled("Required");
				}
				else
				{
					ImGui::TextUnformatted("Enabled");
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
}
