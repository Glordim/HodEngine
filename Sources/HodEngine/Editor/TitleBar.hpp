#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Rect.hpp>

namespace hod::inline editor
{
	/// @brief Custom title bar of the main window (the OS one is hidden, see DesktopWindow::SetCustomTitleBar)
	class HOD_EDITOR_API TitleBar
	{
	public:
		/// @brief Draw the window controls (minimize, maximize/restore, close) at the right of the bar, in the current ImGui window,
		/// and register the bar as the main window drag area, except the controls and the interactiveAreas
		/// @param bar Title bar, screen space
		/// @param interactiveAreas Items of the bar that must stay clickable (menus, buttons...), screen space
		/// @param canMaximize Show the maximize/restore button
		/// @return Screen X where the window controls start
		static float Draw(const Rect& bar, const Vector<Rect>& interactiveAreas, bool canMaximize = true);

		/// @brief Width taken by the window controls at the right of the bar, to lay out the rest of the bar before calling Draw
		static float GetWindowControlsWidth(bool canMaximize = true);

		/// @brief Title bar at the top of the current ImGui window (one covering the whole main window, like the project browser),
		/// with the title on the left. The cursor is moved below it.
		/// @param title
		/// @param canMaximize Show the maximize/restore button
		static void DrawAtTopOfWindow(const char* title, bool canMaximize = false);
	};
}
