#include "EditorApplication.h"

#include <HodEngine/Editor/src/Editor.h>

using namespace HOD;

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool EditorApplication::Init(int argc, char** argv)
{
	bool platformApplicationResult = PlatformApplication::Init(argc, argv);
	if (platformApplicationResult == false)
	{
		return false;
	}

	Editor::CreateInstance();
	if (Editor::GetInstance()->Init(GetWindow()) == false)
	{
		return false;
	}

	return true;
}
