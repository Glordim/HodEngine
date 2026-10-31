#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/UIComponentCustomComponentDrawer.hpp"

#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Game/Components/UIComponent.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <cstring>

namespace hod::inline editor
{
	/// @brief
	/// @param reflectedObject
	/// @return
	bool UIComponentCustomComponentDrawer::OnDrawInspector(EditorReflectedObject& reflectedObject)
	{
		// Every property for a multi-selection mixing the modes
		bool showScreen = false;
		bool showWorld = false;
		bool overrideScaler = true;
		for (void* instance : reflectedObject.GetInstances())
		{
			const UIComponent* uiComponent = static_cast<const UIComponent*>(instance);
			showScreen |= (uiComponent->GetRenderMode() == UIComponent::RenderMode::Screen);
			showWorld |= (uiComponent->GetRenderMode() == UIComponent::RenderMode::World);
			overrideScaler &= uiComponent->GetOverrideScaler();
		}

		bool changed = false;
		for (EditorReflectedProperty* property : reflectedObject.GetProperties())
		{
			const char* name = property->GetReflectionProperty()->GetName();
			bool        screenOnly = std::strcmp(name, "SortingOrder") == 0 || std::strcmp(name, "_overrideScaler") == 0 || std::strcmp(name, "_scaler") == 0;
			bool        worldOnly = std::strcmp(name, "PixelsPerUnit") == 0;
			if ((screenOnly && showScreen == false) || (worldOnly && showWorld == false))
			{
				continue;
			}

			bool disabled = std::strcmp(name, "_scaler") == 0 && overrideScaler == false;
			ImGui::BeginDisabled(disabled);
			changed |= PropertyDrawer::DrawProperty(*property);
			ImGui::EndDisabled();
		}
		return changed;
	}
}
