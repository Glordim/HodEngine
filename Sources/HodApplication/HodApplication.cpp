#include "HodApplication.hpp"

#include <HodEngine/Editor/Editor.hpp>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif

_SingletonOverrideConstructor(HodApplication)
{

}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool HodApplication::Init(const hod::ArgumentParser& argumentParser)
{
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

#if defined(HOD_EDITOR)
	hod::editor::Editor::CreateInstance();
	if (hod::editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}
#else
	// Start game
#endif

#if defined(PLATFORM_WINDOWS)
	HINSTANCE pongDLL = LoadLibrary("Pong.dll");
	if (!pongDLL)
	{
		//std::cout << "could not load the dynamic library" << std::endl;
		return false;
	}

	using initFunction = int(*)();

	// resolve function address here
	initFunction func = (initFunction)GetProcAddress(pongDLL, "Init");
	if (!func)
	{
		//std::cout << "could not locate the function" << std::endl;
		return false;
	}

	func();
#endif

	return true;
}
