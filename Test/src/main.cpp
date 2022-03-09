#include <iostream>

#include <algorithm>
#include <cstring>
#include <vector>

#include "MyApplication.h"

#include <HodEngine/Application/src/VideoSettings.h>
#include <HodEngine/Application/src/ScreenHelper.h>


#include <HodEngine/Core/Src/UID.h>

#include <HodEngine/Core/Src/GenericManager.h>
#include <HodEngine/Core/Src/Output.h>
#include <HodEngine/Core/Src/ArgumentParser.h>
#include <HodEngine/Core/Src/StringConversion.h>

#include <HodEngine/Game/Src/ActorReflection.h>
#include <HodEngine/Game/Src/ComponentReflection.h>

using namespace HOD;


//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	uint16_t toolPort = 0;
	void* toolParent = 0;

	CORE::ArgumentParser argParser;
	argParser.Register('\0', "toolDump", "Request dump reflection data (used by the editor) : [dir where store dumped files]");
	argParser.Register('\0', "toolParent", "Embed the Application as child (used by the editor) : [parent window handle]");
	argParser.Register('\0', "toolPort", "Run a Tcp server to comunicate with tools (used by the editor) : [port number]");
	if (argParser.Parse(argc, argv) == true)
	{
		const char* dumpPath = argParser.GetValue("toolDump");
		if (dumpPath != nullptr)
		{
			if (GAME::ActorReflection::GetInstance()->DumpToDir(dumpPath) == false)
			{
				return EXIT_FAILURE;
			}
			if (GAME::ComponentReflection::GetInstance()->DumpToDir(dumpPath) == false)
			{
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}

		const char* toolParentString = argParser.GetValue("toolParent");
		if (toolParentString != nullptr)
		{
			int64_t toolParentId;
			if (CORE::StringConversion::StringToInt64(toolParentString, toolParentId) == false)
			{
				return EXIT_FAILURE;
			}
			toolParent = reinterpret_cast<void*>(toolParentId);
		}

		const char* toolPortString = argParser.GetValue("toolPort");
		if (toolPortString != nullptr)
		{
			if (CORE::StringConversion::StringToUInt16(toolParentString, toolPort) == false)
			{
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		return EXIT_FAILURE;
	}


	MyApplication app;

	if (app.Init(argc, argv) == false)
	{
		return false;
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
