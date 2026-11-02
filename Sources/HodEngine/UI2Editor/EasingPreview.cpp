#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/EasingPreview.hpp"

#include <HodEngine/Core/Math.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param easing
	void DrawEasingPreview(ui2::AnimationKey::Easing easing)
	{
		constexpr int   SampleCount = 64;
		constexpr float Width = 140.0f;
		constexpr float CurveHeight = 70.0f; // from 0 to 1, the overshoots added
		constexpr float Padding = 6.0f;

		// The factor at the end is the next key's value, whatever the easing (Constant's step)
		float values[SampleCount + 1];
		float min = 0.0f;
		float max = 1.0f;
		for (int index = 0; index <= SampleCount; ++index)
		{
			float t = (float)index / SampleCount;
			values[index] = (index == SampleCount) ? 1.0f : ui2::AnimationKey::Ease(easing, t);
			min = Min(min, values[index]);
			max = Max(max, values[index]);
		}

		float  height = CurveHeight * (max - min) + Padding * 2.0f;
		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImGui::Dummy(ImVec2(Width, height));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		float       left = origin.x + Padding;
		float       right = origin.x + Width - Padding;
		float       bottom = origin.y + height - Padding;
		auto        toY = [bottom, min](float value) { return bottom - (value - min) * CurveHeight; };

		drawList->AddRectFilled(origin, ImVec2(origin.x + Width, origin.y + height), ImGui::GetColorU32(ImGuiCol_FrameBg), 3.0f);
		// 0 and 1 (the two keys' values), linear for reference
		drawList->AddLine(ImVec2(left, toY(0.0f)), ImVec2(right, toY(0.0f)), IM_COL32(255, 255, 255, 40));
		drawList->AddLine(ImVec2(left, toY(1.0f)), ImVec2(right, toY(1.0f)), IM_COL32(255, 255, 255, 40));
		drawList->AddLine(ImVec2(left, toY(0.0f)), ImVec2(right, toY(1.0f)), IM_COL32(255, 255, 255, 25));

		ImVec2 points[SampleCount + 1];
		for (int index = 0; index <= SampleCount; ++index)
		{
			points[index] = ImVec2(left + (right - left) * index / SampleCount, toY(values[index]));
		}
		drawList->AddPolyline(points, SampleCount + 1, IM_COL32(120, 190, 255, 255), 2.0f);
		drawList->AddCircleFilled(points[0], 3.0f, IM_COL32(230, 230, 230, 255));
		drawList->AddCircleFilled(points[SampleCount], 3.0f, IM_COL32(230, 230, 230, 255));

		if (easing == ui2::AnimationKey::Easing::Smooth)
		{
			ImGui::TextDisabled("Between two keys alone.\nWith keys around, goes\nthrough them without a break.");
		}
	}
}
