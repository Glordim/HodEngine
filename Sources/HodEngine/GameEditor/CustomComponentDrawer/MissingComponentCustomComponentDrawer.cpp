#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/MissingComponentCustomComponentDrawer.hpp"

#include <HodEngine/Game/Components/MissingComponent.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param reflectedObject
	/// @return
	bool MissingComponentCustomComponentDrawer::OnDrawInspector(EditorReflectedObject& reflectedObject)
	{
		const MissingComponent* component = static_cast<const MissingComponent*>(reflectedObject.GetInstance());
		if (component == nullptr)
		{
			return false;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
		ImGui::TextWrapped("Unknown component type (%llu): its module is disabled or missing. Its data are kept as they are and saved back.",
		                   static_cast<unsigned long long>(component->GetMissingType()));
		ImGui::PopStyleColor();
		return false;
	}
}
