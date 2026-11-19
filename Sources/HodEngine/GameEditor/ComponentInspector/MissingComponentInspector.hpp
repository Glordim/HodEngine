#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp"

#include <HodEngine/Game/Components/MissingComponent.hpp>

namespace hod::inline editor
{
	/// @brief A MissingComponent: says its data are kept as they are, and why it is there
	class HOD_GAME_EDITOR_API MissingComponentInspector : public ComponentInspector<MissingComponent>
	{
	protected:
		bool OnDraw(const Vector<MissingComponent*>& missingComponents, EditorReflectedObject& reflectedObject) override;
	};
}
