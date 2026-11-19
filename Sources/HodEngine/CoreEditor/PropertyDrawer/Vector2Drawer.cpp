#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/Vector2Drawer.hpp"
#include <HodEngine/Editor/DrawHelper.hpp>

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param values
	/// @return
	bool Vector2Drawer::OnDraw(const PropertyValues<Vector2>& values)
	{
		// Each component on its own: the values edited together may differ on one only
		Vector2 value = values.Get();
		VectorFieldOptions options;
		options._mixedComponents = (values.IsMixed([](const Vector2& vector) { return vector.GetX(); }) ? 1 : 0) |
		                           (values.IsMixed([](const Vector2& vector) { return vector.GetY(); }) ? 2 : 0);
		DrawHelper::ApplyVectorTraits(values.GetBase(), 2, options); // Its labels, its components unused

		uint8_t editedComponents = 0;
		if (DrawHelper::DrawVector2(value, options, &editedComponents) == false)
		{
			return false;
		}

		values.Edit(
			[&](Vector2& edited)
			{
				if (editedComponents & 1)
				{
					edited.SetX(value.GetX());
				}
				if (editedComponents & 2)
				{
					edited.SetY(value.GetY());
				}
			});
		return true;
	}
}
