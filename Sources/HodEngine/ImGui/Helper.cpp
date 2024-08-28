#include "HodEngine/ImGui/Pch.hpp"
#include "Helper.hpp"

namespace hod
{
	ImVec2 CalculateButtonSize(const char* label)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		return ImGui::CalcItemSize(ImVec2(0,0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
	}
}
