#include "HodEngine/Editor/Pch.hpp"
#include "Vector4CustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Core/Math/Vector4.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool Vector4CustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize("X").x;
		availableWidth -= CalculateButtonSize("Y").x;
		availableWidth -= CalculateButtonSize("Z").x;
		availableWidth -= CalculateButtonSize("W").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;

		Vector4 value = *editorReflectedProperty.GetObject<Vector4>();
		float x = value.GetX();
		float y = value.GetY();
		float z = value.GetZ();
		float w = value.GetW();

		ImGui::PushID(&value);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText("X", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText("Y", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##y", ImGuiDataType_Float, &y, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		FramedText("Z", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##z", ImGuiDataType_Float, &z, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		FramedText("W", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.24f);
		changed |= DragScalar("##w", ImGuiDataType_Float, &w, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::PopID();

		if (changed == true)
		{
			editorReflectedProperty.SetObject(Vector4(x, y, z, w));
		}
		return changed;
	}
}
