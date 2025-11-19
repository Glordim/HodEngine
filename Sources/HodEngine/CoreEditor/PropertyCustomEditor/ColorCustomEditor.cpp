#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/ColorCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Core/Color.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

namespace hod::editor
{
	/// @brief
	/// @param instance
	/// @return
	bool ColorCustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		Color value = *editorReflectedProperty.GetObject<Color>();
		changed |= ColorCustomEditor::Draw(value);
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}

	/// @brief
	/// @param value
	/// @return
	bool ColorCustomEditor::Draw(Color& value)
	{
		bool  changed = false;
		float r = value.r;
		float g = value.g;
		float b = value.b;
		float a = value.a;

		ImGui::PushID(&value);

		/*
		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize("R").x;
		availableWidth -= CalculateButtonSize("G").x;
		availableWidth -= CalculateButtonSize("B").x;
		availableWidth -= CalculateButtonSize("A").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText("R", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##r", ImGuiDataType_Float, &r, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText("G", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##g", ImGuiDataType_Float, &g, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		FramedText("B", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##b", ImGuiDataType_Float, &b, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		FramedText("A", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##a", ImGuiDataType_Float, &a, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);
		*/
		ImGui::SetNextItemWidth(-1);
		float rgba[4];
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
		if (ImGui::ColorEdit4("#color", rgba, ImGuiColorEditFlags_AlphaPreviewHalf))
		{
			changed = true;
			r = rgba[0];
			g = rgba[1];
			b = rgba[2];
			a = rgba[3];
		}

		ImGui::PopID();

		if (changed)
		{
			value.r = r;
			value.g = g;
			value.b = b;
			value.a = a;
		}

		return changed;
	}
}
