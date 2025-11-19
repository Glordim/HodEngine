#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::editor
{
	bool DrawHelper::DrawVector2(Vector2& value, const char* labelX, const char* labelY)
	{
		bool changed = false;
		float x = value.GetX();
		float y = value.GetY();

		ImGui::PushID(&value);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize(labelX).x;
		availableWidth -= CalculateButtonSize(labelY).x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;
		
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText(labelX, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText(labelY, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= DragScalar("##y", ImGuiDataType_Float, &y, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::PopID();

		if (changed)
		{
			value.SetX(x);
			value.SetY(y);
		}

		return changed;
	}

	bool DrawHelper::DrawVector4(Vector4& value, const char* labelX, const char* labelY, const char* labelZ, const char* labelW)
	{
		bool changed = false;
		float x = value.GetX();
		float y = value.GetY();
		float z = value.GetZ();
		float w = value.GetW();

		ImGui::PushID(&value);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize(labelX).x;
		availableWidth -= CalculateButtonSize(labelY).x;
		availableWidth -= CalculateButtonSize(labelZ).x;
		availableWidth -= CalculateButtonSize("W").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText(labelX, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText(labelY, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##y", ImGuiDataType_Float, &y, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		FramedText(labelZ, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##z", ImGuiDataType_Float, &z, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		FramedText(labelW, ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##w", ImGuiDataType_Float, &w, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::PopID();

		if (changed)
		{
			value.SetX(x);
			value.SetY(y);
			value.SetZ(z);
			value.SetW(w);
		}

		return changed;
	}
}
