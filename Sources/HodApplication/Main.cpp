#include "HodApplication.hpp"

#include <HodEngine/Core/ArgumentParser.hpp>

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

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
		app->Terminate();
		return 1;
	}

	app->Run();

	app->Terminate();

	HodApplication::DestroyInstance();

	return 0;
}
