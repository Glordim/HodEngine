#include "HodEngine/Editor/MainBar.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Application/Application.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/Application/PlatformDialog.hpp>

namespace hod::editor
{
	/// @brief 
	void MainBar::Draw()
	{
		static bool ImguiDemo = false;
		if (ImguiDemo)
		{
			ImGui::ShowDemoWindow(&ImguiDemo);
		}

		if (ImGui::BeginMenu("File") == true)
		{
			if (ImGui::MenuItem("Save", "CTRL+S") == true)
			{
				Editor::GetInstance()->Save();
			}

			if (ImGui::MenuItem("Save scene As", "CTRL+S") == true)
			{
				Editor::GetInstance()->SaveSceneAs();
			}

			if (ImGui::MenuItem("Quit") == true)
			{
				application::Application::GetInstance()->Quit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit") == true)
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window") == true)
		{
			if (ImGui::MenuItem("Asset Browser") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
			}
			if (ImGui::MenuItem("Inspector") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
			}
			if (ImGui::MenuItem("Viewport") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();
			}
			if (ImGui::MenuItem("Hierachy") == true)
			{
				imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
			}
			if (ImGui::MenuItem("ImGui demo"))
			{
				ImguiDemo = !ImguiDemo;
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Hot Reload") == true)
		{
			application::Application::GetInstance()->ReloadDll();
		}

		const ImGuiStyle& style = ImGui::GetStyle();
		float groupWidth = CalculateButtonSize(ICON_MDI_PLAY).x + style.ItemSpacing.x + CalculateButtonSize(ICON_MDI_PAUSE).x + style.ItemSpacing.x + CalculateButtonSize(ICON_MDI_SKIP_NEXT).x;
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - groupWidth * 0.5f);

		Editor* editor = Editor::GetInstance();
		if (editor->IsPlaying() == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_PLAY, ImVec2(0, 42));
			ImGui::PopStyleColor();
			if (pressed == true)
			{
				editor->Stop();
			}
		}
		else if (ImGui::Button(ICON_MDI_PLAY, ImVec2(0, 42)) == true)
		{
			editor->Play();
		}

		if (editor->IsPaused() == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 42));
			ImGui::PopStyleColor();
			if (pressed == true)
			{
				editor->Resume();
			}
		}
		else if (ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 42)) == true)
		{
			editor->Pause();
		}

		ImGui::BeginDisabled(editor->IsPlaying() == false);
		if (ImGui::Button(ICON_MDI_SKIP_NEXT, ImVec2(0, 42)) == true)
		{
			editor->PlayNextFrame();
		}
		ImGui::EndDisabled();
	}
}
