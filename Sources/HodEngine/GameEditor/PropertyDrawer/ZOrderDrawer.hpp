#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline game
{
	class ZOrder;
}

namespace hod::inline editor
{
	/// @brief Its layer and its order in the layer, on one line
	class HOD_GAME_EDITOR_API ZOrderDrawer : public PropertyDrawer<ZOrder>
	{
	protected:
		bool OnDraw(const PropertyValues<ZOrder>& values) override;
	};
}
