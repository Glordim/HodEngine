#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentInspector/MissingComponentInspector.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param missingComponents
	/// @param reflectedObject
	/// @return
	bool MissingComponentInspector::OnDraw(const Vector<MissingComponent*>& missingComponents, EditorReflectedObject& /*reflectedObject*/)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
		ImGui::TextWrapped("Unknown component type (%llu): its module is disabled or missing. Its data are kept as they are and saved back.",
		                   static_cast<unsigned long long>(missingComponents[0]->GetMissingType()));
		ImGui::PopStyleColor();
		return false;
	}
}
