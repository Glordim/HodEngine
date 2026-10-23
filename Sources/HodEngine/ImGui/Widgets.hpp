#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "DearImGui/imgui.h"
#include "DearImGui/imgui_internal.h"

namespace hod::inline imgui
{
	HOD_IMGUI_API void FramedText(const char* label, ImDrawFlags drawFlags = ImDrawFlags_RoundAll);
	/// @brief Button whose frame can be rounded on some corners only (to glue several widgets into one unified round rect, like FramedText/DragScalar do).
	/// @param active drawn with the "pressed" color while true (toggle state)
	/// @return true when pressed
	HOD_IMGUI_API bool FramedButton(const char* label, bool active = false, ImDrawFlags drawFlags = ImDrawFlags_RoundAll);
	/// @brief Plain clickable text (breadcrumb, link): underlined on hover, highlighted while pressed. The label is drawn as is ("##" is not parsed).
	/// @return true when pressed
	HOD_IMGUI_API bool LinkText(const char* label);
	HOD_IMGUI_API bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundAll);

	HOD_IMGUI_API bool ImageTextButton(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize, const char* label, ImVec2 size_arg = ImVec2(0.0f, 0.0f), ImGuiButtonFlags flags = 0, ImDrawFlags drawFlags = ImDrawFlags_RoundAll);

	/// @brief Inspector color field: swatch filling the item width (opaque color + alpha strip), click opens a picker popup (original/current, RGB/HSV/Hex, recent colors, Escape reverts),
	/// editable hex input on the right (hidden when the item is too narrow), Ctrl+C / Ctrl+V on the hovered swatch or right click copy/paste the hex value,
	/// drag & drop compatible with ImGui color widgets. The label is used for the ID and the tooltip only.
	/// @param col RGBA, or RGB with ImGuiColorEditFlags_NoAlpha
	/// @param flags supported: NoAlpha, HDR, NoInputs (no hex input), NoTooltip, NoDragDrop, NoOptions
	/// @return true when the color changed
	HOD_IMGUI_API bool ColorField(const char* label, float* col, ImGuiColorEditFlags flags = 0);

	HOD_IMGUI_API void PreserveAspectImage(ImTextureID image, ImVec2 imageSize, ImVec2 resizedSize);
}
