#include "EditorApplication.h"

#include <HodEngine/Core/ArgumentParser.h>

//#include <stdint.h>
//#include <stdlib.h>

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	EditorApplication* app = EditorApplication::CreateInstance();

	hod::core::ArgumentParser argumentParser(argc, argv);
	if (app->Init(argumentParser) == false)
	{
		return 1;
	}

	app->Run();

	EditorApplication::DestroyInstance();

	return 0;
}
