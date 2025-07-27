#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Application/Application.hpp>

#include <PortableFileDialogs/portable-file-dialogs.h>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(ProjectBrowser, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	ProjectBrowser::ProjectBrowser()
	{
		std::filesystem::path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		if (std::filesystem::exists(projectsPath) == true)
		{
			Document document;
			DocumentReaderJson jsonReader;
			if (jsonReader.Read(document, projectsPath) == true)
			{
				Serializer::Deserialize(_recentProjects, document.GetRootNode());
			}
		}
	}

	/// @brief 
	void ProjectBrowser::DrawContent()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("ProjectBrowser", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize) == true)
		{
			ImGui::Text("Project Browser");
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 140.0f, 0.0f);
			if (ImGui::Button("Open") == true)
			{
				pfd::settings::verbose(true);
				pfd::open_file dialog("Open HodProject");
				//while (dialog.ready() == false)
				{
				}
				auto result = dialog.result();
				if (result.empty() == false)
				{
					std::filesystem::path path = result[0];
					//std::filesystem::path path = window::GetOpenFileDialog();
					if (path.empty() == false)
					{
						Editor::GetInstance()->OpenProject(path);
					}
				}
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 0, 255));
			bool newProjectButtonClicked = ImGui::Button("New project");
			ImGui::PopStyleColor(1);
			if (newProjectButtonClicked == true)
			{
				std::filesystem::path path = window::GetFolderDialog();
				if (path.empty() == false)
				{
					Editor::GetInstance()->CreateProject(path);
				}
			}

			if (ImGui::BeginTable("ProjectListTable", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg) == true)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 100.0f);
				ImGui::TableSetupColumn("##OpenDelete", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 100.0f);
				ImGui::TableHeadersRow();

				auto itEnd = _recentProjects._projectsPath.end();
				auto it = _recentProjects._projectsPath.begin();
				while (it != itEnd)
				{
					ImGui::PushID(it->c_str());
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					std::filesystem::path path(it->data());
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(path.stem().string().c_str());
					ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 0.9f);
					ImGui::BeginDisabled();
					ImGui::TextUnformatted(it->data());
					ImGui::EndDisabled();
					ImGui::PopFont();
					//ImGui::Text("C:\\users\\glordim\\Desltop\\Dev\\Toto");

					ImGui::TableNextColumn();
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted("a month ago");

					ImGui::TableNextColumn();
					if (ImGui::Button("Open") == true)
					{
						if (Editor::GetInstance()->OpenProject(*it) == false)
						{
							exit(-1);
						}
						/*
						String arguments = "--ProjectPath ";
						arguments += *it;
						if (Process::Create(FileSystem::GetExecutablePath().string(), arguments, true) == true)
						{
							application::Application::GetInstance()->Quit();
						}
						*/
					}
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 0, 0, 255));
					bool deleteButtonClicked = ImGui::Button(ICON_MDI_CLOSE);
					ImGui::PopStyleColor(1);
					if (deleteButtonClicked == true)
					{
						it = _recentProjects._projectsPath.erase(it);

						std::filesystem::path projectsPath = FileSystem::GetUserSettingsPath();
						projectsPath /= ("HodEngine");
						projectsPath /= ("Project.json");

						Document writeDocument;
						Serializer::Serialize(_recentProjects, writeDocument.GetRootNode());

						std::filesystem::create_directories(projectsPath.parent_path());
						DocumentWriterJson jsonWriter;
						jsonWriter.Write(writeDocument, projectsPath);
					}
					else
					{
						++it;
					}
					ImGui::PopID();
				}

				ImGui::EndTable();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(1);
	}
}
