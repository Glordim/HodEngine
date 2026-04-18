#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Rebuildable.hpp"

#include <HodEngine/Game/Component.hpp>

namespace hod::ui
{
	class HOD_UI_API Rebuildable : public Component
	{
		REFLECTED_CLASS(Rebuildable, Component)

	public:

		void	SetDirty();

		virtual bool Rebuild() = 0;
	};
}
