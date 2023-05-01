#include "EditorApplication.h"

#include <HodEngine/Editor/Editor.h>

using namespace hod;

_SingletonOverrideConstructor(EditorApplication)
{

}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool EditorApplication::Init(const hod::core::ArgumentParser& argumentParser)
{
	bool result = PlatformApplication::Init(argumentParser);
	if (result == false)
	{
		return false;
	}

	editor::Editor::CreateInstance();
	if (editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}

	return true;
}
