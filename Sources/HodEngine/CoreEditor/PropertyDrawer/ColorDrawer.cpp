#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/ColorDrawer.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Widgets.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param values
	/// @return
	bool ColorDrawer::OnDraw(const PropertyValues<Color>& values)
	{
		const Color& value = values.Get();
		float        rgba[4] = {value.r, value.g, value.b, value.a};

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, values.IsMixed());
		ImGui::SetNextItemWidth(-1);
		bool changed = ColorField("##color", rgba);
		ImGui::PopItemFlag();

		if (changed)
		{
			values.Set(Color(rgba[0], rgba[1], rgba[2], rgba[3]));
		}
		return changed;
	}
}
