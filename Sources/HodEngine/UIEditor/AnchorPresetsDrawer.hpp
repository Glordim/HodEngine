#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline math
{
	class Vector2;
}

namespace hod::inline editor
{
	/// @brief Anchor-preset picker (a 3x3 grid of buttons plus stretch variants, Unity
	/// RectTransform-style) letting the user set AnchorMin/AnchorMax/Pivot in one click instead of
	/// editing the 3 vectors by hand. Used by the UI prefab inspector (UIPrefabEditorInspectorWindow).
	class HOD_UI_EDITOR_API AnchorPresetsDrawer
	{
	public:

		static bool Draw(const ImVec2& size, Vector2& anchorMin, Vector2& anchorMax, Vector2& pivot);
	};
}
