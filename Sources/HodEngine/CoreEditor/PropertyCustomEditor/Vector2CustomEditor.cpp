#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

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
		changed |= DrawHelper::DrawVector2(value);
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}
}
