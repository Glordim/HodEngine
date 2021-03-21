#include <iostream>

#include <algorithm>
#include <cstring>
#include <vector>

#include "MyApplication.h"

#include <HodEngine/Application/src/VideoSettings.h>
#include <HodEngine/Application/src/ScreenHelper.h>


#include <HodEngine/Core/Src/UID.h>
#include <Windows.h>

#include <HodEngine/Core/Src/GenericManager.h>
#include <HodEngine/Core/Src/Output.h>
using namespace HOD;

/*
class TestManager : public CORE::GenericManager<char>
{

};

void AddDataCallback(char* data)
{
	OUTPUT_MESSAGE("ADD Data 2 %s ", data);
}

void RemoveDataCallback(char* data)
{
	OUTPUT_MESSAGE("REMOVE Data 2 %s ", data);
}*/

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	/*
	TestManager manager;

	char* testValue = "toto";

	CORE::UID uid = CORE::UID::GenerateUID();

	CORE::Signal<char*>::Slot AddDataSlot(&AddDataCallback);
	CORE::Signal<char*>::Slot RemoveDataSlot(&RemoveDataCallback);
	manager.RegisterAddData(AddDataSlot);
	manager.RegisterRemoveData(RemoveDataSlot);

	manager.AddData(uid, testValue);
	OUTPUT_MESSAGE("ADD Data %u ", manager.Count());

	OUTPUT_MESSAGE("%u ",manager.Count());

	manager.RemoveData(uid);*/

	/* TEST UID
	CORE::UID test = CORE::UID::GenerateUID();

	std::string string = test.ToString();

	OutputDebugString(string.c_str());

	CORE::UID test2 = CORE::UID::FromString(string.c_str());

	std::string string2 = test2.ToString();

	OutputDebugString(string2.c_str());

	if (test == test2)
	{
		OutputDebugString("Equal !");
	}

	return false;*/






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
