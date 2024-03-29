#include "Vector2CustomEditor.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool Vector2CustomEditor::Draw(void* instance, ReflectionPropertyObject* property)
	{
		bool changed = false;

		Vector2 value = *static_cast<Vector2*>(property->GetValue(instance));
		float x = value.GetX();
		float y = value.GetY();

		ImGui::PushID(property->GetValue(instance));

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(property->GetDisplayName().c_str());

		ImGui::SameLine(valuePos);
			
		float availableWidth = ImGui::GetColumnWidth();
		availableWidth -= ImGui::CalcTextSize("X").x;
		availableWidth -= ImGui::CalcTextSize("Y").x;
		availableWidth -= ImGui::GetStyle().ItemSpacing.x * 3;
		ImGui::AlignTextToFramePadding();
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= ImGui::DragFloat("##x", &x);
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(availableWidth * 0.5f);
		changed |= ImGui::DragFloat("##y", &y);

		ImGui::PopID();

		if (changed == true)
		{
			value.SetX(x);
			value.SetY(y);
			property->SetValue(instance, &value);
		}
		return changed;
	}
}
