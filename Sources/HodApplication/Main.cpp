#include "HodApplication.hpp"

#include <HodEngine/Core/ArgumentParser.hpp>

//#include <stdint.h>
//#include <stdlib.h>

#include "Main.hpp"

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	return HodMain(argc, argv);
}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int HodMain(int argc, char** argv)
{
	hod::ArgumentParser argumentParser(argc, argv);

	HodApplication* app = HodApplication::CreateInstance();
	if (app->Init(argumentParser) == false)
	{
		return 1;
	}

	app->Run();

	HodApplication::DestroyInstance();

	return 0;
}
