#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/VersionControl/VersionControlStatus.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief What the asset browser shows of the version control: the badges of its items, its toolbar (connection, Pull, settings),
	/// its "Version Control/..." contextual menu actions and their confirmations. Main thread, from the status cache only
	class HOD_EDITOR_API VersionControlGui
	{
	public:
		/// @brief Something to show: not Unknown/Unchanged/Ignored, or a lock, or out of date
		static bool HasBadge(const VersionControlStatus& status);

		/// @brief On an item's image (min/max): the state bottom left, out of date top left, the lock top right
		static void DrawBadges(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, const VersionControlStatus& status);
		/// @brief As text, on the current line (folder tree)
		static void DrawInlineBadges(const VersionControlStatus& status);
		/// @brief The tooltip of a hovered item, if it has a badge. folder: its state is the one of what it contains
		static void DrawTooltip(const VersionControlStatus& status, bool folder);

		/// @brief The toolbar's buttons (status/settings menu, Pull), on the current line. Nothing if no provider is registered
		static float CalcToolbarWidth();
		static void  DrawToolbar();

		/// @brief The confirmations (Revert, Pull with unsaved tabs): inside a window, once per frame
		static void DrawModals();
		/// @brief Reverted once confirmed (DrawModals)
		static void RequestRevert(Vector<Path> paths);

		/// @brief By AssetBrowserWindow::PopulateContextualMenuActions
		static void RegisterContextualMenuActions();

		/// @brief nullptr for the states without a badge (Unknown, Ignored, Unchanged)
		static const char* GetStateIcon(VersionControlState state);
		static ImU32       GetStateColor(VersionControlState state);
		/// @brief Lower case ("modified")
		static const char* GetStateLabel(VersionControlState state);
	};
}
