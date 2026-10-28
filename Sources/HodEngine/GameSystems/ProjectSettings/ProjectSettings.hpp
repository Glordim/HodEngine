#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline gamesystems
{
	/// @brief Base of a project wide settings class: one reflected class per module or game (ui2::UISettings...),
	/// registered in ProjectSettingsManager which owns its single instance. The editor edits them (Edit > Project
	/// Settings) and saves each in <Project>/ProjectSettings/<TypeName>.json, the build ships them with the game data.
	class HOD_GAME_SYSTEMS_API ProjectSettings
	{
		REFLECTED_CLASS_NO_PARENT(ProjectSettings)

	public:
		ProjectSettings() = default;
		virtual ~ProjectSettings() = default;
	};
}
