#include "GameApplication.hpp"

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool GameApplication::Init(const hod::ArgumentParser& argumentParser)
{
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

	return true;
}
