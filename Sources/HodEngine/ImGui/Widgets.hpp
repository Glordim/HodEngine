#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "DearImGui/imgui.h"
#include "DearImGui/imgui_internal.h"

namespace hod::inline imgui
{
	HOD_IMGUI_API void FramedText(const char* label, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
	/// @brief Button whose frame can be rounded on some corners only (to glue several widgets into one unified round rect, like FramedText/DragScalar do).
	/// @param active drawn with the "pressed" color while true (toggle state)
	/// @return true when pressed
	HOD_IMGUI_API bool FramedButton(const char* label, bool active = false, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);
	HOD_IMGUI_API bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);

	HOD_IMGUI_API bool ImageTextButton(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize, const char* label, ImVec2 size_arg = ImVec2(0.0f, 0.0f), ImGuiButtonFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll);

	HOD_IMGUI_API void PreserveAspectImage(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize);
}
