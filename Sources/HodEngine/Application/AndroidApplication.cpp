#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/AndroidApplication.hpp"

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace hod::inline application
{
	bool AndroidApplication::Run(android_app* androidApp)
	{
		_androidApp = androidApp;
		return RunInternal();
	}

	void AndroidApplication::ConfigureFileSystem(FileSystemConfig& fileSystemConfig)
	{
		fileSystemConfig._assetManager = _androidApp->activity->assetManager;
	}

	void AndroidApplication::ConfigureDisplayManager(DisplayManagerConfig& displayManagerConfig)
	{
		displayManagerConfig._androidApp = _androidApp;
	}
}
