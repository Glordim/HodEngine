#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline editor
{
	struct Task;

	/// @brief
	class HOD_EDITOR_API TaskTrackerWindow : public imgui::Window
	{
		REFLECTED_CLASS(TaskTrackerWindow, imgui::Window)

	public:
		TaskTrackerWindow();
		~TaskTrackerWindow() override = default;

	public:
		bool Draw() override;
		void DrawContent() override;

	private:
		void DrawTask(const Task& task) const;
	};
}
