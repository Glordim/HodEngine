#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector2CustomPropertyDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::inline editor
{
	/// @brief
	/// @param editorReflectedProperty
	/// @return
	bool Vector2CustomPropertyDrawer::Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		// Each component on its own: the instances edited together may differ on one only
		Vector2 value = *editorReflectedProperty.GetObject<Vector2>();
		uint8_t mixedComponents = 0;
		for (uint32_t index = 1; index < editorReflectedProperty.GetInstances().Size(); ++index)
		{
			const Vector2& other = *editorReflectedProperty.GetObjectOf<Vector2>(index);
			mixedComponents |= (other.GetX() != value.GetX() ? 1 : 0) | (other.GetY() != value.GetY() ? 2 : 0);
		}

		uint8_t editedComponents = 0;
		if (DrawHelper::DrawVector2(value, "X", "Y", true, true, mixedComponents, &editedComponents))
		{
			editorReflectedProperty.EditObjects<Vector2>(
				[&](Vector2& object)
				{
					if (editedComponents & 1)
					{
						object.SetX(value.GetX());
					}
					if (editedComponents & 2)
					{
						object.SetY(value.GetY());
					}
				});
			changed = true;
		}
		return changed;
	}
}
