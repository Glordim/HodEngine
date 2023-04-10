#include "EditorApplication.h"

#include <HodEngine/Editor/src/Editor.h>

using namespace hod;

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool EditorApplication::Init(const hod::core::ArgumentParser& argumentParser)
{
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

	editor::Editor::CreateInstance();
	if (editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}

	return true;
}
