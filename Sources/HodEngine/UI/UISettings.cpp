#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/UISettings.hpp"

#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(UISettings)
	{
		HOD_PROPERTY(_canvasScaler);
	}

	/// @brief
	/// @return
	const UISettings* UISettings::Get()
	{
		ProjectSettingsManager* projectSettingsManager = ProjectSettingsManager::GetInstance();
		return projectSettingsManager != nullptr ? projectSettingsManager->Get<UISettings>() : nullptr;
	}
}
