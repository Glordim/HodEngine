#include "EditorApplication.h"

#include <HodEngine/Editor/src/Editor.h>

using namespace hod;

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

	editor::Editor::CreateInstance();
	if (editor::Editor::GetInstance()->Init() == false)
	{
		return false;
	}

	return true;
}
