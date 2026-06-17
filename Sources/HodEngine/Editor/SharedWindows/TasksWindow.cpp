#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/SharedWindows/TasksWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(TasksWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	TasksWindow::TasksWindow()
	{
		SetFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	}

	/// @brief
	/// @return
	bool TasksWindow::Draw()
	{
		if (ImGui::Begin(GetIdentifier(), nullptr, GetFlags()) == true)
		{
			ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			ImGui::PopClipRect();
			ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
			DrawContent();
		}
		ImGui::End();
		return true;
	}

	/// @brief
	void TasksWindow::DrawContent()
	{
		ImGui::TextUnformatted("toto");
	}
}
