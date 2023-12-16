#if defined(HOD_EDITOR)

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
		ImGui::Columns(2);
		ImGui::TextUnformatted(property->GetName());
		ImGui::NextColumn();
		
		ImGui::TextUnformatted("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth() * 0.35f);
		changed |= ImGui::DragFloat("##x", &x);
		ImGui::SameLine();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth() * 0.35f);
		changed |= ImGui::DragFloat("##y", &y);

		ImGui::NextColumn();
		ImGui::Columns(1);
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

#endif
