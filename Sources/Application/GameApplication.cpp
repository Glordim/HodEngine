#include "GameApplication.h"

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool GameApplication::Init(int argc, char** argv)
{
	bool platformApplicationResult = PlatformApplication::Init(argc, argv);
	if (platformApplicationResult == false)
	{
		return false;
	}

	return true;
}
