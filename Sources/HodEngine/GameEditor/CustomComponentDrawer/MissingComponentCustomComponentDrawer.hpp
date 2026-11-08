#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::inline editor
{
	/// @brief A MissingComponent: says its data are kept as they are, and why it is there
	class HOD_GAME_EDITOR_API MissingComponentCustomComponentDrawer : public CustomComponentDrawer
	{
	public:
		bool OnDrawInspector(EditorReflectedObject& reflectedObject) override;
	};
}
