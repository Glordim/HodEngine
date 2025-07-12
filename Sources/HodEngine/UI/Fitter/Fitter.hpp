#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/IRebuildable.hpp"

#include <HodEngine/Game/Component.hpp>

namespace hod::ui
{
	class HOD_UI_API Fitter : public game::Component, IRebuildable
	{
		REFLECTED_CLASS(Fitter, game::Component)
	};
}
