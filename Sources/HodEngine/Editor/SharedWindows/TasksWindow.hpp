#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API TasksWindow : public imgui::Window
	{
		REFLECTED_CLASS(TasksWindow, imgui::Window)

	public:
		TasksWindow();
		~TasksWindow() override = default;

	public:
		bool Draw() override;
		void DrawContent() override;
	};
}
