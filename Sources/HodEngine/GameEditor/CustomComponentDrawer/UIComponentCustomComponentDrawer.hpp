#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline editor
{
	/// @brief Only the properties of the UIComponent's render mode: the sorting order and the scaler for Screen, the pixels per
	/// unit for World. The scaler is greyed out while the prefab's is used.
	class HOD_GAME_EDITOR_API UIComponentCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

											UIComponentCustomComponentDrawer() = default;
											UIComponentCustomComponentDrawer(const UIComponentCustomComponentDrawer&) = delete;
											UIComponentCustomComponentDrawer(UIComponentCustomComponentDrawer&&) = delete;
											~UIComponentCustomComponentDrawer() override = default;

		UIComponentCustomComponentDrawer	operator = (const UIComponentCustomComponentDrawer&) = delete;
		UIComponentCustomComponentDrawer	operator = (UIComponentCustomComponentDrawer&&) = delete;

	public:

		bool								OnDrawInspector(EditorReflectedObject& reflectedObject) override;
	};
}
