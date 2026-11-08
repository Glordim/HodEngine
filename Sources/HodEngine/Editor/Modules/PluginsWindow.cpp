#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Modules/PluginsWindow.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Modules/ModuleGraph.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>


namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(PluginsWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

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
			ImGuiManager::GetInstance()->OpenWindow<PluginsWindow>();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	PluginsWindow::PluginsWindow()
	: _disabledModules(Project::GetInstance()->GetDisabledModules())
	{
		SetTitle("Plugins");
	}

	/// @brief Enabling a module enables what it needs too, disabling it shows the ones needing it disabled (see ModuleGraph::Resolve)
	/// @param name
	/// @param enabled
	void PluginsWindow::SetEnabled(const String& name, bool enabled)
	{
		if (enabled == false)
		{
			if (Contains(_disabledModules, name) == false)
			{
				_disabledModules.PushBack(name);
			}
			return;
		}

		for (uint32_t index = 0; index < _disabledModules.Size(); ++index)
		{
			if (_disabledModules[index] == name)
			{
				_disabledModules.Erase(index);
				break;
			}
		}
		if (const ModuleGraph::Module* module = Project::GetInstance()->GetModuleGraph().Find(name))
		{
			for (const String& dependency : module->_manifest._dependencies)
			{
				SetEnabled(dependency, true);
			}
		}
	}

	/// @brief
	/// @return
	bool PluginsWindow::HasChanges() const
	{
		const Vector<String>& current = Project::GetInstance()->GetDisabledModules();
		if (current.Size() != _disabledModules.Size())
		{
			return true;
		}
		for (const String& name : _disabledModules)
		{
			if (Contains(current, name) == false)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief Saved in the project, then the editor restarts on it: the libraries of the modules are loaded with the project, and the
	/// game module links them (rebuilt then)
	/// @param saveTabs
	void PluginsWindow::Apply(bool saveTabs)
	{
		if (saveTabs && Editor::GetInstance()->Save() == false)
		{
			OUTPUT_ERROR("Plugins: some tabs couldn't be saved, the editor doesn't restart");
			return;
		}
		if (Project::GetInstance()->SetDisabledModules(_disabledModules) == false)
		{
			return;
		}
		Editor::GetInstance()->Restart();
	}

	/// @brief
	void PluginsWindow::DrawContent()
	{
		const ModuleGraph& current = Project::GetInstance()->GetModuleGraph();
		ModuleGraph        preview = current; // The pending changes resolved: what applying them gives
		preview.Resolve(_disabledModules);

		ImGui::TextDisabled("Engine modules. A disabled module isn't loaded by the editor nor linked in the game.");

		const ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;
		float                 footerHeight = ImGui::GetFrameHeightWithSpacing() * 2.0f + ImGui::GetStyle().ItemSpacing.y;
		if (ImGui::BeginTable("Modules", 3, tableFlags, ImVec2(0.0f, -footerHeight)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("##Enabled", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetFrameHeight());
			ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch, 1.0f);
			ImGui::TableHeadersRow();

			for (const ModuleGraph::Module& module : preview.GetModules())
			{
				const ModuleManifest&      manifest = module._manifest;
				const ModuleGraph::Module* currentModule = current.Find(manifest._name);
				bool                       changed = currentModule != nullptr && currentModule->_enabled != module._enabled;
				bool                       failed = Editor::GetInstance()->HasModuleFailed(manifest._name);

				ImGui::PushID(manifest._name.CStr());
				ImGui::TableNextRow();

				// What the user asks for: a module asked for can still be disabled (it needs a disabled one, see the state)
				ImGui::TableNextColumn();
				bool asked = Contains(_disabledModules, manifest._name) == false;
				ImGui::BeginDisabled(manifest._required);
				if (ImGui::Checkbox("##Enabled", &asked))
				{
					SetEnabled(manifest._name, asked);
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
					ImGui::Text("Needs: %s", manifest._dependencies.Empty() ? "-" : Join(manifest._dependencies).CStr());
					if (manifest._provides.Empty() == false)
					{
						ImGui::Text("Provides: %s", Join(manifest._provides).CStr());
					}
					ImGui::Text("Runtime library: %s", manifest._runtimeLibrary.Empty() ? "- (part of the engine)" : manifest._runtimeLibrary.CStr());
					ImGui::Text("Editor library: %s", manifest._editorLibrary.Empty() ? "-" : manifest._editorLibrary.CStr());
					ImGui::EndTooltip();
				}

				ImGui::TableNextColumn();
				if (module._enabled == false)
				{
					ImGui::TextDisabled("%s", module._disabledReason.CStr());
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
					ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), module._enabled ? "(will be enabled)" : "(will be disabled)");
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
			_disabledModules = Project::GetInstance()->GetDisabledModules();
		}
		ImGui::EndDisabled();
		if (hasChanges)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("The editor restarts, the game module is rebuilt");
		}

		if (ImGui::BeginPopupModal(ApplyModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			bool dirtyTabs = Editor::GetInstance()->HasDirtyTabs();
			ImGui::TextUnformatted("The editor restarts on the project to load the modules, the game module is rebuilt.");
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
