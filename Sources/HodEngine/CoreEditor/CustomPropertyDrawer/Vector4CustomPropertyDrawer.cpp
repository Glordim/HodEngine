#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector4CustomPropertyDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param instance
	/// @return
	bool Vector4CustomPropertyDrawer::Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		// Each component on its own: the instances edited together may differ on one only
		Vector4 value = *editorReflectedProperty.GetObject<Vector4>();
		uint8_t mixedComponents = 0;
		for (uint32_t index = 1; index < editorReflectedProperty.GetInstances().Size(); ++index)
		{
			const Vector4& other = *editorReflectedProperty.GetObjectOf<Vector4>(index);
			mixedComponents |= (other.GetX() != value.GetX() ? 1 : 0) | (other.GetY() != value.GetY() ? 2 : 0) | (other.GetZ() != value.GetZ() ? 4 : 0) |
			                   (other.GetW() != value.GetW() ? 8 : 0);
		}

		uint8_t editedComponents = 0;
		if (DrawHelper::DrawVector4(value, "X", "Y", "Z", "W", mixedComponents, &editedComponents))
		{
			editorReflectedProperty.EditObjects<Vector4>(
				[&](Vector4& object)
				{
					if (editedComponents & 1)
					{
						object.SetX(value.GetX());
					}
					if (editedComponents & 2)
					{
						object.SetY(value.GetY());
					}
					if (editedComponents & 4)
					{
						object.SetZ(value.GetZ());
					}
					if (editedComponents & 8)
					{
						object.SetW(value.GetW());
					}
				});
			changed = true;
		}
		return changed;
	}
}
