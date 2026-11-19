#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline game
{
	class ZOrder;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API ZOrderDrawer : public PropertyDrawer<ZOrder>
	{
	public:
		static bool Draw(ZOrder& value);

	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};
}
