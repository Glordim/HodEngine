#include "HodApplication.hpp"

#include <HodEngine/Core/ArgumentParser.hpp>

//#include <stdint.h>
//#include <stdlib.h>

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	hod::ArgumentParser argumentParser(argc, argv);

	HodApplication app;
	if (app.Init(argumentParser) == false)
	{
		return 1;
	}

	app.Run();

	return 0;
}
