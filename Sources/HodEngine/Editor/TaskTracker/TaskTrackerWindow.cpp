#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTrackerWindow.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"
#include "HodEngine/ImGui/Helper.hpp"
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(TaskTrackerWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	TaskTrackerWindow::TaskTrackerWindow()
	{
		SetFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		uint64_t taskId = 0;
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();

		taskId = taskTracker.CreateTask("Basic task", "Basic description");
		taskTracker.UpdateTaskProgress(taskId, 0.02f);

		taskId = taskTracker.CreateTask("Cooking", "Description blabla");
		taskTracker.UpdateTaskProgress(taskId, 0.25f, 43.0f);

		taskId = taskTracker.CreateTask("Import", "");
		taskTracker.UpdateTaskProgress(taskId, 0.42f);

		taskId = taskTracker.CreateTask("Task finished", "Description toto");
		taskTracker.UpdateTaskProgress(taskId, 1.0f);
		taskTracker.UpdateTaskStatus(taskId, TaskStatus::Succeeded);

		taskId = taskTracker.CreateTask("Task finished", "Description toto");
		taskTracker.UpdateTaskProgress(taskId, 0.7f);
		taskTracker.UpdateTaskStatus(taskId, TaskStatus::Failed);

		taskId = taskTracker.CreateTask("Infinite", "Description");
		taskTracker.UpdateTaskProgress(taskId, -1.0f);

		taskId = taskTracker.CreateTask("Cancelable", "Description", [](){ return false; });
		taskTracker.UpdateTaskProgress(taskId, -1.0f);

		taskId = taskTracker.CreateTask("Canceled", "Description toto");
		taskTracker.UpdateTaskProgress(taskId, 0.2f);
		taskTracker.UpdateTaskStatus(taskId, TaskStatus::Canceled);
	}

	/// @brief
	/// @return
	bool TaskTrackerWindow::Draw()
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
	void TaskTrackerWindow::DrawContent()
	{
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();

		if (ImGui::Button("Clear inactive"))
		{
			// todo
		}
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);

		if (ImGui::BeginChild("Tasks", ImVec2(0.0f, 0.0f), 0, ImGuiWindowFlags_NoBackground))
		{
			taskTracker.LockTasks();
			for (const Task* task : taskTracker.GetTasks())
			{
				DrawTask(*task);
			}
			taskTracker.UnlockTasks();
		}
		ImGui::EndChild();
	}

	void TaskTrackerWindow::DrawTask(const Task& task) const
	{
		bool finished = (task.status == TaskStatus::Succeeded ||
			task.status == TaskStatus::Failed ||
			task.status == TaskStatus::Canceled);

		if (finished)
		{
			ImVec4 bgCol = ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
			bgCol.x *= 0.75f;
			bgCol.y *= 0.75f;
			bgCol.z *= 0.75f;
			//bgCol.w *= 0.5f;
			ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
		}
		if (ImGui::BeginChild(ImGui::GetID(&task), ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders))
		{
			float availableWidth = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().WindowPadding.x;
			static constexpr float progressBarWidth = 100.0f;
			float progressBarPosX = availableWidth - progressBarWidth;

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(task.name.CStr());

			if (finished)
			{
				ImGui::SameLine(progressBarPosX - ImGui::GetStyle().ItemSpacing.x - CalculateButtonSize(ICON_MDI_DELETE).x);
				ImGui::Button(ICON_MDI_DELETE);
			}
			else if (task.cancelCallback)
			{
				ImGui::SameLine(progressBarPosX - ImGui::GetStyle().ItemSpacing.x - CalculateButtonSize(ICON_MDI_CANCEL).x);
				ImGui::Button(ICON_MDI_CANCEL);
			}

			ImGui::SameLine(progressBarPosX);
			if (finished)
			{
				ImGui::BeginDisabled();
			}
			ImGui::AlignTextToFramePadding();
			if (task.percent < 0.0f)
			{
				ImGui::ProgressBar(-1.0f * ImGui::GetTime(), ImVec2(progressBarWidth, 0.0f));
			}
			else
			{
				ImGui::ProgressBar(task.percent, ImVec2(progressBarWidth, 0.0f));
			}
			if (finished)
			{
				ImGui::EndDisabled();
			}

			if (task.status == TaskStatus::Succeeded)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
				ImGui::TextUnformatted(ICON_MDI_CHECK);
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::BeginDisabled();
				ImGui::TextUnformatted("Succeeded");
			}
			else if (task.status == TaskStatus::Failed)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::TextUnformatted(ICON_MDI_CLOSE);
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::BeginDisabled();
				ImGui::TextUnformatted("Failed");
			}
			else if (task.status == TaskStatus::Canceled)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 10, 255));
				ImGui::TextUnformatted(ICON_MDI_CANCEL);
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::BeginDisabled();
				ImGui::TextUnformatted("Canceled");
			}
			else
			{
				ImGui::BeginDisabled();
				ImGui::TextUnformatted(task.description.CStr());
			}
			if (task.remainingTime >= 0.0f)
			{
				String timeLeft = String::Format("%us left", (uint32_t)task.remainingTime);
				ImGui::SameLine(availableWidth - ImGui::CalcTextSize(timeLeft.CStr()).x);
				ImGui::TextUnformatted(timeLeft.CStr());
			}
			ImGui::EndDisabled();
		}
		ImGui::EndChild();
		if (finished)
		{
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
	}
}
