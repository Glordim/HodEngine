#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/GameSystems/ProjectSettings/ProjectSettings.hpp>

#include "HodEngine/UI/Canvas.hpp"

namespace hod::inline ui
{
	/// @brief UI project settings (registered by ui::RegisterBuiltin).
	class HOD_UI_API UISettings : public ProjectSettings
	{
		REFLECTED_CLASS(UISettings, ProjectSettings)

	public:
		/// @return nullptr when not registered
		static const UISettings* Get();

	public:
		// Scaling of every canvas that doesn't override it (see Canvas::SetOverrideScaler): one design resolution for the
		// whole project, so a canvas unit has the same size on every screen and prefabs can be nested in any of them.
		CanvasScaler _canvasScaler;
	};
}
