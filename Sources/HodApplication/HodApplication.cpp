#include "HodApplication.hpp"

#include <HodEngine/Editor/Editor.hpp>

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

	return true;
}
