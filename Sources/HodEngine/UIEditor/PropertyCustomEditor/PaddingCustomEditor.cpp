#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/PropertyCustomEditor/PaddingCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"

#include "HodEngine/UI/Padding.hpp"

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
	bool PaddingCustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		ui::Padding value = *editorReflectedProperty.GetObject<ui::Padding>();
		changed |= DrawHelper::DrawVector4(value._vector4, "Left", "Bottom", "Right", "Top");
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		return changed;
	}
}
