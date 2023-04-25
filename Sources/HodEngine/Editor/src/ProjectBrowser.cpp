#include "ProjectBrowser.h"
#include "AssetDatabase.h"
#include "Editor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/src/DearImGui/imgui.h>
#include <HodEngine/ImGui/src/DearImGui/imgui_internal.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

#include "HodEngine/Editor/src/Editor.h"
#include "HodEngine/Editor/src/Project.h"

#include <HodEngine/Application/src/PlatformDialog.h>

#include "HodEngine/Core/Src/FileSystem.h"
#include "HodEngine/Core/Src/Stream/FileStream.h"
#include "HodEngine/Core/Src/Document/Document.h"
#include "HodEngine/Core/Src/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Src/Document/DocumentWriterJson.h"

namespace hod::editor
{
	/// @brief 
	ProjectBrowser::ProjectBrowser()
	{
		std::filesystem::path projectsPath = core::FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		core::Document document;
		core::DocumentReaderJson jsonReader;
		jsonReader.Read(document, projectsPath);

		RecentProjects::GetReflectionDescriptor()->DeserializeFromDocument(_recentProjects, document.GetRootNode());
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
				std::filesystem::path path = application::dialog::GetOpenFileDialog();
				Editor::GetInstance()->OpenProject(path);
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 0, 255));
			bool newProjectButtonClicked = ImGui::Button("New project");
			ImGui::PopStyleColor(1);
			if (newProjectButtonClicked == true)
			{

			}

			ImGui::Separator();

			if (ImGui::BeginTable("ProjectListTable", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg) == true)
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 100.0f);
				ImGui::TableSetupColumn("##OpenDelete", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 100.0f);
				ImGui::TableHeadersRow();

				for (int index = 0; index < _projectsPath.size(); ++index)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Text(_projectsPath[index].string().c_str());
					//ImGui::Text("C:\\users\\glordim\\Desltop\\Dev\\Toto");

					ImGui::TableNextColumn();
					ImGui::Text("a month ago");

					ImGui::TableNextColumn();
					if (ImGui::Button("Open") == true)
					{
						Editor::GetInstance()->OpenProject(_projectsPath[index]);
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
