#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "DearImGui/imgui.h"
#include "DearImGui/imgui_internal.h"

namespace hod
{
	HOD_IMGUI_API ImVec2 CalculateButtonSize(const char* label);
}
