#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include <HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp>
#include <HodEngine/UIComponents/Components/UIComponent.hpp>

namespace hod::inline editor
{
	/// @brief Only the properties of the UIComponent's render mode: the sorting order and the scaler for Screen, the pixels per
	/// unit for World. The scaler is greyed out while the prefab's is used.
	class HOD_UI_EDITOR_API UIComponentInspector : public ComponentInspector<UIComponent>
	{
	protected:
		bool OnDraw(const Vector<UIComponent*>& uiComponents, EditorReflectedObject& reflectedObject) override;
	};
}
