#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Plugins/PluginsWindow.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Plugins/PluginGraph.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>


namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(PluginsWindow, reflectionDescriptor) {}

	namespace
	{
		constexpr const char* ApplyModalName = "Restart the editor###PluginsApply";

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

		/// @brief A, B, C
		String Join(const Vector<String>& names)
		{
			String result;
			for (const String& name : names)
			{
				if (result.Empty() == false)
				{
					result += ", ";
				}
				result += name;
			}
			return result;
		}
	}

	/// @brief
	void PluginsWindow::Open()
	{
		imgui::Window* window = ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor());
		if (window == nullptr)
		{
			ImGuiManager::GetInstance()->OpenWindow<PluginsWindow>()->CenterOnCurrentWindow();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	PluginsWindow::PluginsWindow()
	: _disabledPlugins(Project::GetInstance()->GetDisabledPlugins())
	{
		SetTitle("Plugins");
		SetInitialSize(ImVec2(720.0f, 420.0f));
	}

	/// @brief
	/// @return
	bool PluginsWindow::HasChanges() const
	{
		const Vector<String>& current = Project::GetInstance()->GetDisabledPlugins();
		if (current.Size() != _disabledPlugins.Size())
		{
			return true;
		}
		for (const String& name : _disabledPlugins)
		{
			if (Contains(current, name) == false)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief Saved in the project, then the editor restarts on it: the modules of the plugins are loaded with the project, and the
	/// project's runtime module links them (rebuilt then)
	/// @param saveTabs
	void PluginsWindow::Apply(bool saveTabs)
	{
		if (saveTabs && Editor::GetInstance()->Save() == false)
		{
			OUTPUT_ERROR("Plugins: some tabs couldn't be saved, the editor doesn't restart");
			return;
		}
		if (Project::GetInstance()->SetDisabledPlugins(_disabledPlugins) == false)
		{
			return;
		}
		Editor::GetInstance()->Restart();
	}

	/// @brief
	void PluginsWindow::DrawContent()
	{
		const PluginGraph& current = Project::GetInstance()->GetPluginGraph();
		PluginGraph        preview = current; // The pending changes resolved: what applying them gives
		preview.Resolve(_disabledPlugins);

		ImGui::TextDisabled("The engine's plugins, then the project's own. A disabled plugin isn't loaded by the editor nor linked in the game.");

		const ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;
		float                 footerHeight = ImGui::GetFrameHeightWithSpacing() * 2.0f + ImGui::GetStyle().ItemSpacing.y;
		if (ImGui::BeginTable("Plugins", 3, tableFlags, ImVec2(0.0f, -footerHeight)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("##Enabled", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetFrameHeight());
			ImGui::TableSetupColumn("Plugin", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableHeadersRow();

			for (const PluginGraph::Plugin& plugin : preview.GetPlugins())
			{
				const PluginManifest&      manifest = plugin._manifest;
				const PluginGraph::Plugin* currentPlugin = current.Find(manifest._name);
				bool                       changed = currentPlugin != nullptr && currentPlugin->_enabled != plugin._enabled;
				bool                       failed = Editor::GetInstance()->HasPluginFailed(manifest._name);

				ImGui::PushID(manifest._name.CStr());
				ImGui::TableNextRow();

				// What the user asks for: a plugin asked for can still be disabled (it needs a disabled one, see the state)
				ImGui::TableNextColumn();
				bool asked = Contains(_disabledPlugins, manifest._name) == false;
				ImGui::BeginDisabled(manifest._required);
				if (ImGui::Checkbox("##Enabled", &asked))
				{
					current.SetEnabled(_disabledPlugins, manifest._name, asked);
				}
				ImGui::EndDisabled();
				if (manifest._required && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_ForTooltip))
				{
					ImGui::SetTooltip("Required: can't be disabled");
				}

				ImGui::TableNextColumn();
				ImGui::BeginGroup();
				ImGui::TextUnformatted(manifest._name.CStr());
				if (manifest._description.Empty() == false)
				{
					ImGui::SameLine();
					ImGui::TextDisabled("%s", manifest._description.CStr());
				}
				ImGui::EndGroup();
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
				{
					ImGui::BeginTooltip();
					ImGui::Text("Needs: %s", plugin._needsAll ? "every enabled plugin (built against them)" : manifest._dependencies.Empty() ? "-" : Join(manifest._dependencies).CStr());
					if (manifest._provides.Empty() == false)
					{
						ImGui::Text("Provides: %s", Join(manifest._provides).CStr());
					}
					ImGui::Text("Runtime modules: %s", manifest._runtimeModules.Empty() ? "- (part of the engine)" : Join(manifest._runtimeModules).CStr());
					ImGui::Text("Editor module: %s", manifest._editorModule.Empty() ? "-" : manifest._editorModule.CStr());
					ImGui::EndTooltip();
				}

				ImGui::TableNextColumn();
				if (plugin._enabled == false && asked)
				{
					// Asked for but not enabled all the same (another provides the same thing, or it needs a disabled plugin): a warning
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " %s", plugin._disabledReason.CStr());
				}
				else if (plugin._enabled == false)
				{
					ImGui::TextDisabled("%s", plugin._disabledReason.CStr());
				}
				else if (failed && changed == false)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), ICON_MDI_ALERT " Failed to load (see the console)");
				}
				else if (manifest._required)
				{
					ImGui::TextDisabled("Required");
				}
				else
				{
					ImGui::TextUnformatted("Enabled");
				}
				if (changed)
				{
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), plugin._enabled ? "(will be enabled)" : "(will be disabled)");
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		for (const String& error : preview.GetErrors())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), ICON_MDI_ALERT " %s", error.CStr());
		}

		bool hasChanges = HasChanges();
		ImGui::BeginDisabled(hasChanges == false);
		if (ImGui::Button("Apply and restart"))
		{
			ImGui::OpenPopup(ApplyModalName);
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert"))
		{
			_disabledPlugins = Project::GetInstance()->GetDisabledPlugins();
		}
		ImGui::EndDisabled();
		if (hasChanges)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("The editor restarts, the project's modules are rebuilt");
		}

		CenterNextWindowOnCurrentWindow();
		if (ImGui::BeginPopupModal(ApplyModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			bool dirtyTabs = Editor::GetInstance()->HasDirtyTabs();
			ImGui::TextUnformatted("The editor restarts on the project to load the plugins, the project's modules are rebuilt.");
			if (dirtyTabs)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), ICON_MDI_ALERT " Some tabs have unsaved changes.");
				if (ImGui::Button("Save and restart"))
				{
					ImGui::CloseCurrentPopup();
					Apply(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Restart without saving"))
				{
					ImGui::CloseCurrentPopup();
					Apply(false);
				}
			}
			else if (ImGui::Button("Restart"))
			{
				ImGui::CloseCurrentPopup();
				Apply(false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
