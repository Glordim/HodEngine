#include "HodEditor.hpp"

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

	HodEditor* app = HodEditor::CreateInstance();
	if (app->Init(argumentParser) == false)
	{
		return 1;
	}

	app->Run();

	app->Terminate();

	HodEditor::DestroyInstance();

	return 0;
}
