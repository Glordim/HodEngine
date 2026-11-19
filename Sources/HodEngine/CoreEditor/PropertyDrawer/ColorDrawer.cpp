#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/ColorDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param instance
	/// @return
	bool ColorDrawer::OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= ReflectionDrawer::BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		Color value = *editorReflectedProperty.GetObject<Color>();
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, editorReflectedProperty.HasDifferentValues());
		bool edited = ColorDrawer::Draw(value);
		ImGui::PopItemFlag();
		if (edited)
		{
			editorReflectedProperty.SetObject(value);
			changed = true;
		}
		return changed;
	}

	/// @brief
	/// @param value
	/// @return
	bool ColorDrawer::Draw(Color& value)
	{
		float rgba[4] = {value.r, value.g, value.b, value.a};

		ImGui::PushID(&value);
		ImGui::SetNextItemWidth(-1);
		const bool changed = ColorField("##color", rgba);
		ImGui::PopID();

		if (changed)
		{
			value = Color(rgba[0], rgba[1], rgba[2], rgba[3]);
		}
		return changed;
	}
}
