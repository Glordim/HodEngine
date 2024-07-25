#include "HodEngine/Editor/MissingGameModuleModal.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"
#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(MissingGameModuleModal, "MissingGameModuleModal", true)

	/// @brief 
	MissingGameModuleModal::MissingGameModuleModal()
	{
		application::DesktopApplication* application = application::DesktopApplication::GetInstance();
		window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
		mainWindow->SetSize(320, 200);
		mainWindow->CenterToScreen();
		mainWindow->SetVisible(true);
	}

	/// @brief 
	bool MissingGameModuleModal::Draw()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		if (ImGui::Begin("MissingGameModuleModal", nullptr, ImGuiWindowFlags_NoDecoration))
		{
			DrawContent();
		}
		ImGui::End();

		ImGui::PopStyleVar();

		return true;
	}

	/// @brief 
	void MissingGameModuleModal::DrawContent()
	{
		ImGui::TextUnformatted("Missing 'Game' module");
		ImGui::TextUnformatted("Do you want to generate it?");
		if (ImGui::Button("Generate"))
		{
			if (Project::GetInstance()->GenerateGameModuleCMakeList() == false)
			{

			}
			else if (Project::GetInstance()->BuildGameModule() == false)
			{

			}
			else
			{
				Editor::GetInstance()->LoadEditor();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
		{
			application::DesktopApplication* application = application::DesktopApplication::GetInstance();
			application->Quit();
		}
	}
}
