#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Application/PlatformDialog.hpp>

#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "portable-file-dialogs.h"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ProjectBrowser, "Project Browser", true)

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
	void ProjectBrowser::Draw()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("ProjectBrowser", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize) == true)
		{
			ImGui::Text("Projects");
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 140.0f);
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
					//std::filesystem::path path = application::dialog::GetOpenFileDialog();
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
				std::filesystem::path path = application::dialog::GetFolderDialog();
				if (path.empty() == false)
				{
					Editor::GetInstance()->CreateProject(path);
				}
			}

			ImGui::Separator();

			if (ImGui::BeginTable("ProjectListTable", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg) == true)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 100.0f);
				ImGui::TableSetupColumn("##OpenDelete", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 100.0f);
				ImGui::TableHeadersRow();

				for (int index = 0; index < _recentProjects._projectsPath.size(); ++index)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::TextUnformatted(_recentProjects._projectsPath[index].data());
					//ImGui::Text("C:\\users\\glordim\\Desltop\\Dev\\Toto");

					ImGui::TableNextColumn();
					ImGui::TextUnformatted("a month ago");

					ImGui::TableNextColumn();
					if (ImGui::Button("Open") == true)
					{
						Editor::GetInstance()->OpenProject(_recentProjects._projectsPath[index]);
					}
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(150, 0, 0, 255));
					bool deleteButtonClicked = ImGui::Button("Delete");
					ImGui::PopStyleColor(1);
					if (deleteButtonClicked == true)
					{

					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(1);
	}
}
