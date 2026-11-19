#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline game
{
	class WeakComponentBase;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API WeakComponentDrawer : public PropertyDrawer<WeakComponentBase>
	{
	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};
}
