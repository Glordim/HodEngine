#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector4CustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

#include "HodEngine/Core/Math/Vector4.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool Vector4CustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		Vector4 value = *editorReflectedProperty.GetObject<Vector4>();
		changed |= DrawHelper::DrawVector4(value);
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}
}
