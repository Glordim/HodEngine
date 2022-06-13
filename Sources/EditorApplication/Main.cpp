#include "EditorApplication.h"

//#include <stdint.h>
//#include <stdlib.h>

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	EditorApplication app;
	if (app.Init(argc, argv) == false)
	{
		return 1;
	}

	app.Run();

	return 0;
}
