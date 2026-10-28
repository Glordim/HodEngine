#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include <cstdint>

namespace hod::inline editor
{
	/// @brief Edit > Project Settings: the registered ProjectSettings classes listed on the left, the selected one's
	/// reflected properties on the right, saved to its file once an edit ends.
	class HOD_EDITOR_API ProjectSettingsWindow : public imgui::Window
	{
		REFLECTED_CLASS(ProjectSettingsWindow, imgui::Window)

	public:
		/// @brief Opens the window, or focuses it when already open
		static void Open();

	public:
		ProjectSettingsWindow();

		void DrawContent() override;

	private:
		// Reflection types rather than pointers: a game module's settings go away with it ("Hot Reload")
		uint64_t _selectedType = 0;
		uint64_t _pendingSaveType = 0;
	};
}
