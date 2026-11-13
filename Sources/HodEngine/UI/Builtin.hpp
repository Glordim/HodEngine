#pragma once
#include "HodEngine/UI/Export.hpp"

namespace hod::inline core
{
	class ModuleRegistration;
}

namespace hod::inline ui
{
	/// @brief UI's own nodes, layouts, layout params, interactions and project settings (UISettings), in factories created already
	HOD_UI_API void RegisterBuiltin(ModuleRegistration& registration);
}
