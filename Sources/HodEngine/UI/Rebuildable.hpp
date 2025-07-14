#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Rebuildable.hpp"

#include <HodEngine/Game/Component.hpp>

namespace hod::ui
{
	class HOD_UI_API Rebuildable : public game::Component
	{
		REFLECTED_CLASS(Rebuildable, game::Component)

	public:

		void	SetDirty();

		virtual bool Rebuild() = 0;
	};
}
