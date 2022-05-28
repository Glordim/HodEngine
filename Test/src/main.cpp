#include <iostream>

#include <algorithm>
#include <cstring>
#include <vector>

#include "MyApplication.h"

#include <HodEngine/Application/src/VideoSettings.h>
#include <HodEngine/Application/src/ScreenHelper.h>


#include <HodEngine/Core/Src/UID.h>

using namespace HOD;

#include <Windows.h>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	MessageBox(
		NULL,
		(LPCSTR)L"Resource not available\nDo you want to try again?",
		(LPCSTR)L"Account Details",
		MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
	);

	MyApplication app;
	MyApplication::InitResult initResult = app.Init(argc, argv);

	if (initResult == MyApplication::InitResult::Failure)
	{
		return EXIT_FAILURE;
	}
	if (initResult == MyApplication::InitResult::Quit)
	{
		return EXIT_SUCCESS;
	}

	int selectedMonitor = 0;

	// For Debug
	if (APPLICATION::ScreenHelper::GetMonitorCount() > 1)
	{
		//selectedMonitor = 1;
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

	if (app.Run(CORE::UID::INVALID_UID) == false)
	{
		return 1;
	}

	return 0;
}
