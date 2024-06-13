#include "Vector2CustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool Vector2CustomEditor::Draw(EditorReflectedObject& reflectedObject)
	{
		bool changed = false;
		void* instance = reflectedObject.GetInstance();

		if (reflectedObject.IsOverride() == true)
		{
			float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
		}

		Vector2 value = *static_cast<Vector2*>(instance);
		float x = value.GetX();
		float y = value.GetY();

		ImGui::PushID(instance);

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ReflectionPropertyObject* reflectionProperty = static_cast<ReflectionPropertyObject*>(reflectedObject.GetSourceProperty()->GetReflectionProperty());

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(reflectionProperty->GetDisplayName().c_str());

		ImGui::SameLine(valuePos);

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

		if (changed == true)
		{
			value.SetX(x);
			value.SetY(y);
			reflectionProperty->SetValue(reflectedObject.GetSourceProperty()->GetParent()->GetInstance(), &value);
		}
		return changed;
	}
}
