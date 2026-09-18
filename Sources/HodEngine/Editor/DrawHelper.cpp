#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <algorithm>
#include <cmath>

namespace hod::inline editor
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

	bool DrawHelper::BeginInspectorBlock(const void* id, const char* icon, const char* title, bool* enabled, bool* removeRequested)
	{
		ImGui::PushID(id);
		if (ImGui::BeginChild("InspectorBlock", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders) == false)
		{
			return false;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(0.0f);
		ImGui::SetCursorPosY(0.0f);
		ImGui::PopStyleVar(1);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(0.0f);
		ImGui::Separator();
		ImGui::SetCursorPosX(0.0f);
		ImGui::PopStyleVar(2);

		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = min;
		max.x += ImGui::GetWindowWidth();
		max.y += ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 4;
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));

		min.y += ImGui::GetStyle().FramePadding.y;
		ImGui::SetCursorScreenPos(min);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGuiStorage* storage = ImGui::GetCurrentWindow()->DC.StateStorage;
		ImGuiID       collapsedId = ImGui::GetID("Collapsed");
		bool          collapsed = storage->GetInt(collapsedId, false);

		if (ImGui::ArrowButton("CollapseBlock", collapsed == false ? ImGuiDir_Down : ImGuiDir_Right))
		{
			collapsed = !collapsed;
			storage->SetInt(collapsedId, collapsed);
		}
		ImGui::PopStyleColor(4);

		bool disabled = false;
		if (enabled != nullptr)
		{
			ImGui::SameLine();
			ImGui::Checkbox("##Enabled", enabled);
			disabled = (*enabled == false);
		}

		ImGui::BeginDisabled(disabled);
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(icon);
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(title);
		ImGui::EndDisabled();

		if (removeRequested != nullptr)
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_CLOSE).x + 10.0f, 0.0f);
			ImVec2 buttonPos = ImGui::GetCursorScreenPos();
			ImVec2 mousePos = ImGui::GetMousePos();

			// The close button fades in as the mouse gets close to it.
			float distance = (float)std::sqrt(std::pow(buttonPos.x - mousePos.x, 2) + std::pow(buttonPos.y - mousePos.y, 2));
			distance = std::clamp(distance, 15.0f, 40.0f);
			float alpha = 1.0f - ((distance - 15.0f) / (40.0f - 15.0f));

			ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			textColor.w = alpha;

			ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			buttonColor.w = alpha;

			ImVec4 borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
			borderColor.w = alpha;

			ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::Button(ICON_MDI_CLOSE))
			{
				*removeRequested = true;
			}
			ImGui::PopStyleColor(4);
		}

		min.y = max.y;
		max.x += ImGui::GetWindowWidth();
		ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));

		if (collapsed == false)
		{
			ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 2.0f));
			return true;
		}

		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.y = max.y;
		ImGui::SetCursorScreenPos(pos);
		return false;
	}

	void DrawHelper::EndInspectorBlock()
	{
		ImGui::Dummy(ImVec2(0, 0)); // todo
		ImGui::EndChild();
		ImGui::PopID();
	}
}
