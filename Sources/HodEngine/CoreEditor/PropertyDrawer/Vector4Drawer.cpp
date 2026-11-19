#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/Vector4Drawer.hpp"
#include <HodEngine/Editor/DrawHelper.hpp>

#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param values
	/// @return
	bool Vector4Drawer::OnDraw(const PropertyValues<Vector4>& values)
	{
		// Each component on its own: the values edited together may differ on one only
		Vector4 value = values.Get();
		uint8_t mixedComponents = (values.IsMixed([](const Vector4& vector) { return vector.GetX(); }) ? 1 : 0) |
		                          (values.IsMixed([](const Vector4& vector) { return vector.GetY(); }) ? 2 : 0) |
		                          (values.IsMixed([](const Vector4& vector) { return vector.GetZ(); }) ? 4 : 0) |
		                          (values.IsMixed([](const Vector4& vector) { return vector.GetW(); }) ? 8 : 0);

		uint8_t editedComponents = 0;
		if (DrawHelper::DrawVector4(value, "X", "Y", "Z", "W", mixedComponents, &editedComponents) == false)
		{
			return false;
		}

		values.Edit(
			[&](Vector4& edited)
			{
				if (editedComponents & 1)
				{
					edited.SetX(value.GetX());
				}
				if (editedComponents & 2)
				{
					edited.SetY(value.GetY());
				}
				if (editedComponents & 4)
				{
					edited.SetZ(value.GetZ());
				}
				if (editedComponents & 8)
				{
					edited.SetW(value.GetW());
				}
			});
		return true;
	}
}
