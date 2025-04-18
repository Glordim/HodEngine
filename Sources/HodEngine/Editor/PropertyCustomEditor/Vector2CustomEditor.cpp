#include "HodEngine/Editor/Pch.hpp"
#include "Vector2CustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param editorReflectedProperty 
	/// @return 
	bool Vector2CustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		Vector2 value = *editorReflectedProperty.GetObject<Vector2>();
		changed |= Vector2CustomEditor::Draw(value);
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}

	/// @brief 
	/// @param value 
	/// @return 
	bool Vector2CustomEditor::Draw(Vector2& value)
	{
		bool changed = false;
		float x = value.GetX();
		float y = value.GetY();

		ImGui::PushID(&value);

		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= CalculateButtonSize("X").x;
		availableWidth -= CalculateButtonSize("Y").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;
		
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		FramedText("X", ImDrawFlags_RoundCornersLeft);
		ImGui::PopStyleColor();
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= DragScalar("##x", ImGuiDataType_Float, &x, 1.0f, nullptr, nullptr, nullptr, 0, ImDrawFlags_RoundCornersRight);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		FramedText("Y", ImDrawFlags_RoundCornersLeft);
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
}
