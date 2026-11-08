#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/TitleBar.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <HodEngine/Application/Application.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>

#include <portable-file-dialogs.h>

namespace hod::inline editor
{
	namespace
	{
		const ImVec4 MissingColor(1.0f, 0.6f, 0.2f, 1.0f);
	}

	DESCRIBE_REFLECTED_CLASS(ProjectBrowser, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	ProjectBrowser::ProjectBrowser()
	{
		Path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		if (FileSystem::GetInstance()->Exists(projectsPath) == true)
		{
			Document           document;
			DocumentReaderJson jsonReader;
			if (jsonReader.Read(document, projectsPath) == true)
			{
				Serializer::Deserialize(_recentProjects, document.GetRootNode());
			}
		}
	}

	/// @brief
	bool ProjectBrowser::Draw()
	{
		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(mainViewport->Pos);
		ImGui::SetNextWindowSize(mainViewport->Size);
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("ProjectBrowser", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize) == true)
		{
			TitleBar::DrawAtTopOfWindow("HodEngine");

			if (_newProjectFormShown)
			{
				// Created: the project is opened, the browser closed at the end of the frame (ImGuiManager::CloseAllWindow)
				NewProjectForm::Result result = _newProjectForm.Draw();
				if (result == NewProjectForm::Result::Cancelled)
				{
					ShowNewProjectForm(false);
				}
				else
				{
					ResizeMainWindow(); // To the form, its modules expanded or not
				}
				ImGui::End();
				ImGui::PopStyleVar(1);
				return true;
			}

			ImGui::Text("Project Browser");
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 140.0f, 0.0f);
			if (ImGui::Button("Open") == true)
			{
				pfd::settings::verbose(true);
				pfd::open_file dialog("Open HodProject");
				// while (dialog.ready() == false)
				{
				}
				auto result = dialog.result();
				if (result.empty() == false)
				{
					Path path = result[0].c_str();
					// Path path = GetOpenFileDialog();
					if (path.Empty() == false)
					{
						OpenProject(path);
					}
				}
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 0, 255));
			bool newProjectButtonClicked = ImGui::Button("New project");
			ImGui::PopStyleColor(1);
			if (newProjectButtonClicked == true)
			{
				ShowNewProjectForm(true);
			}

			if (_openError.Empty() == false)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), ICON_MDI_ALERT " %s", _openError.CStr());
			}

			if (ImGui::BeginTable("ProjectListTable", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX) == true)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 100.0f);
				ImGui::TableSetupColumn("##OpenDelete", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 100.0f);
				ImGui::TableHeadersRow();

				// Removed after the table: erasing while iterating left the end iterator stale, a garbage row drawn for a frame
				int32_t removedIndex = -1;
				for (uint32_t index = 0; index < _recentProjects._projectsPath.Size(); ++index)
				{
					const String* it = &_recentProjects._projectsPath[index];
					ImGui::PushID(it->CStr());
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					Path       path(it->CStr());
					const bool missing = FileSystem::GetInstance()->IsRegularFile(path) == false; // Moved, deleted, on a drive not there
					ImGui::AlignTextToFramePadding();
					if (missing)
					{
						ImGui::TextColored(MissingColor, ICON_MDI_ALERT " %s", path.Stem().GetString().CStr());
					}
					else
					{
						ImGui::TextUnformatted(path.Stem().GetString().CStr());
					}
					ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 0.9f);
					ImGui::BeginDisabled();
					ImGui::TextUnformatted(it->CStr());
					ImGui::EndDisabled();
					ImGui::PopFont();

					ImGui::TableNextColumn();
					ImGui::AlignTextToFramePadding();
					if (missing)
					{
						ImGui::TextColored(MissingColor, "Not found");
					}
					else
					{
						ImGui::TextUnformatted("a month ago");
					}

					ImGui::TableNextColumn();
					ImGui::BeginDisabled(missing);
					if (ImGui::Button("Open") == true)
					{
						OpenProject(path);
					}
					ImGui::EndDisabled();
					if (missing && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_ForTooltip))
					{
						ImGui::SetTooltip("%s doesn't exist anymore (moved, deleted, or on a drive not connected)", it->CStr());
					}
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 0, 0, 255));
					bool deleteButtonClicked = ImGui::Button(ICON_MDI_CLOSE);
					ImGui::PopStyleColor(1);
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
					{
						ImGui::SetTooltip("Remove from the list (the project stays on the disk)");
					}
					if (deleteButtonClicked == true)
					{
						removedIndex = static_cast<int32_t>(index);
					}
					ImGui::PopID();
				}

				ImGui::EndTable();

				if (removedIndex >= 0)
				{
					_recentProjects._projectsPath.Erase(static_cast<uint32_t>(removedIndex));

					Path projectsPath = FileSystem::GetUserSettingsPath();
					projectsPath /= ("HodEngine");
					projectsPath /= ("Project.json");

					Document writeDocument;
					Serializer::Serialize(_recentProjects, writeDocument.GetRootNode());

					FileSystem::GetInstance()->CreateDirectories(projectsPath.ParentPath());
					DocumentWriterJson jsonWriter;
					jsonWriter.Write(writeDocument, projectsPath);
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(1);
		return true;
	}

	/// @brief A failure stays in the browser, told above the list (the details in the console)
	/// @param path
	void ProjectBrowser::OpenProject(const Path& path)
	{
		_openError.Clear();
		if (FileSystem::GetInstance()->IsRegularFile(path) == false)
		{
			_openError = String::Format("%s doesn't exist", path.GetString().CStr());
			return;
		}
		if (Editor::GetInstance()->OpenProject(path) == false)
		{
			OUTPUT_ERROR("Unable to open {}", path);
			_openError = String::Format("Unable to open %s", path.GetString().CStr());
		}
	}

	/// @brief The form needs more room than the list: the main window grows while it's shown
	/// @param show
	void ProjectBrowser::ShowNewProjectForm(bool show)
	{
		_newProjectFormShown = show;
		if (show)
		{
			_newProjectForm.Reset(FileSystem::GetUserHomePath());
		}
		ResizeMainWindow();
	}

	/// @brief Only when the height changes (the form's is known once drawn), centered again then
	void ProjectBrowser::ResizeMainWindow()
	{
		uint16_t height = 320;
		if (_newProjectFormShown && _newProjectForm.GetContentHeight() > 0.0f)
		{
			height = static_cast<uint16_t>(_newProjectForm.GetContentHeight() + 0.5f);
		}
		if (height == _mainWindowHeight)
		{
			return;
		}
		_mainWindowHeight = height;
		DesktopWindow* mainWindow = static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow());
		mainWindow->SetSize(800, height);
		mainWindow->CenterToScreen();
	}
}
