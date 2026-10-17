#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TitleBar.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

namespace hod::inline editor
{
	static constexpr float ControlWidth = 46.0f;

	/// @brief
	/// @param id
	/// @param icon
	/// @param pos
	/// @param size
	/// @param hoveredColor
	/// @param activeColor
	/// @return
	static bool WindowControlButton(const char* id, const char* icon, const ImVec2& pos, const ImVec2& size, ImU32 hoveredColor, ImU32 activeColor)
	{
		ImGui::SetCursorScreenPos(pos);
		bool        clicked = ImGui::InvisibleButton(id, size);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (ImGui::IsItemActive())
		{
			drawList->AddRectFilled(pos, pos + size, activeColor);
		}
		else if (ImGui::IsItemHovered())
		{
			drawList->AddRectFilled(pos, pos + size, hoveredColor);
		}
		ImVec2 iconSize = ImGui::CalcTextSize(icon);
		drawList->AddText(ImFloor(pos + (size - iconSize) * 0.5f), ImGui::GetColorU32(ImGuiCol_Text), icon);
		return clicked;
	}

	/// @brief
	/// @param bar
	/// @param interactiveAreas
	/// @param canMaximize
	/// @return
	float TitleBar::Draw(const Rect& bar, const Vector<Rect>& interactiveAreas, bool canMaximize)
	{
		DesktopWindow* mainWindow = static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow());

		ImVec2 barPos(bar._position.GetX(), bar._position.GetY());
		ImVec2 barSize(bar._size.GetX(), bar._size.GetY());
		ImVec2 controlSize(ControlWidth, barSize.y);
		float  controlsWidth = GetWindowControlsWidth(canMaximize);
		ImVec2 controlPos(barPos.x + barSize.x - controlsWidth, barPos.y);
		float  controlsStartX = controlPos.x;

		ImU32 hoveredColor = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		ImU32 activeColor = ImGui::GetColorU32(ImGuiCol_ButtonActive);
		if (WindowControlButton("##Minimize", ICON_MDI_WINDOW_MINIMIZE, controlPos, controlSize, hoveredColor, activeColor))
		{
			mainWindow->Minimize();
		}
		controlPos.x += controlSize.x;

		if (canMaximize)
		{
			bool maximized = mainWindow->IsWindowMaximized();
			if (WindowControlButton("##Maximize", maximized ? ICON_MDI_WINDOW_RESTORE : ICON_MDI_WINDOW_MAXIMIZE, controlPos, controlSize, hoveredColor, activeColor))
			{
				if (maximized)
				{
					mainWindow->Restore();
				}
				else
				{
					mainWindow->Maximize();
				}
			}
			controlPos.x += controlSize.x;
		}

		if (WindowControlButton("##Close", ICON_MDI_WINDOW_CLOSE, controlPos, controlSize, IM_COL32(196, 43, 28, 255), IM_COL32(150, 30, 20, 255)))
		{
			mainWindow->Close();
		}

		// Hit regions are in client space of the main window
		ImVec2  clientOrigin = ImGui::GetMainViewport()->Pos;
		Vector2 clientOffset(-clientOrigin.x, -clientOrigin.y);

		Vector<Rect> clientInteractiveAreas;
		clientInteractiveAreas.Reserve(interactiveAreas.Size() + 1);
		for (const Rect& interactiveArea : interactiveAreas)
		{
			clientInteractiveAreas.push_back(Rect {interactiveArea._position + clientOffset, interactiveArea._size});
		}
		clientInteractiveAreas.push_back(Rect {Vector2(controlsStartX - clientOrigin.x, barPos.y - clientOrigin.y), Vector2(controlsWidth, controlSize.y)});
		mainWindow->SetTitleBarHitRegions(Rect {bar._position + clientOffset, bar._size}, clientInteractiveAreas);

		return controlsStartX;
	}

	/// @brief
	/// @param canMaximize
	/// @return
	float TitleBar::GetWindowControlsWidth(bool canMaximize)
	{
		return ControlWidth * (canMaximize ? 3.0f : 2.0f);
	}

	/// @brief
	/// @param title
	/// @param canMaximize
	void TitleBar::DrawAtTopOfWindow(const char* title, bool canMaximize)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		ImVec2            windowPos = ImGui::GetWindowPos();
		ImVec2            barSize(ImGui::GetWindowWidth(), ImGui::GetFrameHeight() + 8.0f);

		// The window content is clipped inside its padding, the bar goes edge to edge
		ImGui::PushClipRect(windowPos, windowPos + barSize, false);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(windowPos, windowPos + barSize, IM_COL32(20, 20, 20, 255));
		ImVec2 titlePos(windowPos.x + style.WindowPadding.x, windowPos.y + (barSize.y - ImGui::GetFontSize()) * 0.5f);
		drawList->AddText(ImFloor(titlePos), ImGui::GetColorU32(ImGuiCol_TextDisabled), title);

		Draw(Rect {Vector2(windowPos.x, windowPos.y), Vector2(barSize.x, barSize.y)}, {}, canMaximize);

		ImGui::PopClipRect();

		ImGui::SetCursorScreenPos(ImVec2(windowPos.x + style.WindowPadding.x, windowPos.y + barSize.y + style.WindowPadding.y));
	}
}
