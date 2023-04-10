#include "EditorApplication.h"

#include <HodEngine/Core/Src/ArgumentParser.h>

//#include <stdint.h>
//#include <stdlib.h>

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	hod::core::ArgumentParser argumentParser(argc, argv);

	EditorApplication app;
	if (app.Init(argumentParser) == false)
	{
		return 1;
	}

	app.Run();

	return 0;
}
