#pragma once

#include "DearImGui/imgui.h"
#include "DearImGui/imgui_internal.h"

namespace hod
{
	void FramedText(const char* label, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
	bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
}
