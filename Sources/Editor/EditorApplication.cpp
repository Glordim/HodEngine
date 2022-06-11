#include "EditorApplication.h"

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

	Editor::CreateIntance();

	return true;
}
