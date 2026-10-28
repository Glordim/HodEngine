#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/UISettings.hpp"

#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(UISettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &UISettings::_canvasScaler, "CanvasScaler");
	}

	/// @brief
	/// @return
	const UISettings* UISettings::Get()
	{
		ProjectSettingsManager* projectSettingsManager = ProjectSettingsManager::GetInstance();
		return projectSettingsManager != nullptr ? projectSettingsManager->Get<UISettings>() : nullptr;
	}
}
