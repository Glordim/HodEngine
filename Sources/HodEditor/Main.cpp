#include "EditorApplication.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv)
{
	hod::EditorApplication editorApplication;
	if (editorApplication.Run(argc, argv) == false)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
