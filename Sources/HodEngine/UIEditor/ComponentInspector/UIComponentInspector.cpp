#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/ComponentInspector/UIComponentInspector.hpp"

#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"

#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <cstring>

namespace hod::inline editor
{
	/// @brief
	/// @param uiComponents
	/// @param reflectedObject
	/// @return
	bool UIComponentInspector::OnDraw(const Vector<UIComponent*>& uiComponents, EditorReflectedObject& reflectedObject)
	{
		// Every property for a multi-selection mixing the modes
		bool showScreen = false;
		bool showWorld = false;
		bool overrideScaler = true;
		for (const UIComponent* uiComponent : uiComponents)
		{
			showScreen |= (uiComponent->GetRenderMode() == UIComponent::RenderMode::Screen);
			showWorld |= (uiComponent->GetRenderMode() == UIComponent::RenderMode::World);
			overrideScaler &= uiComponent->GetOverrideScaler();
		}

		bool changed = false;
		for (EditorReflectedProperty* property : reflectedObject.GetProperties())
		{
			const char* name = property->GetReflectionProperty()->GetName();
			bool        screenOnly = std::strcmp(name, "_sortingOrder") == 0 || std::strcmp(name, "_overrideScaler") == 0 || std::strcmp(name, "_scaler") == 0;
			bool        worldOnly = std::strcmp(name, "_pixelsPerUnit") == 0;
			if ((screenOnly && showScreen == false) || (worldOnly && showWorld == false))
			{
				continue;
			}

			bool disabled = std::strcmp(name, "_scaler") == 0 && overrideScaler == false;
			ImGui::BeginDisabled(disabled);
			changed |= ReflectionDrawer::DrawProperty(*property);
			ImGui::EndDisabled();
		}
		return changed;
	}
}
