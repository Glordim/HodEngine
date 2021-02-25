#include <iostream>

#include <algorithm>
#include <cstring>
#include <vector>

#include "MyApplication.h"

#include <HodEngine/Application/src/VideoSettings.h>
#include <HodEngine/Application/src/ScreenHelper.h>

using namespace HOD;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	MyApplication app;

	if (app.Init() == false)
	{
		return false;
	}

	int selectedMonitor = 0;

	// For Debug
	if (APPLICATION::ScreenHelper::GetMonitorCount() > 1)
	{
		selectedMonitor = 1;
	}

	APPLICATION::Resolution nativeResolution;

	if (APPLICATION::ScreenHelper::GetNativeResoltion(&nativeResolution, selectedMonitor) == false)
	{
		return 1;
	}

	APPLICATION::VideoSettings videoSettings;
	videoSettings.monitor = selectedMonitor;
	videoSettings.resolution = nativeResolution;
	videoSettings.fullscreenMode = APPLICATION::FullscreenMode::Windowed;

	if (app.CreateWindowAndContext("HodEngine", videoSettings) == false)
	{
		return 1;
	}

	if (app.Run() == false)
	{
		return 1;
	}

	return 0;
}
