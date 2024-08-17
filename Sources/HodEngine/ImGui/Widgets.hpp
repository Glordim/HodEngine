#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "DearImGui/imgui.h"
#include "DearImGui/imgui_internal.h"

namespace hod
{
	HOD_IMGUI_API void FramedText(const char* label, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
	HOD_IMGUI_API bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);

	HOD_IMGUI_API bool ImageTextButton(ImTextureID image, ImVec2 imageSize, const char* label, ImVec2 size_arg = ImVec2(0.0f, 0.0f), ImGuiButtonFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
}
