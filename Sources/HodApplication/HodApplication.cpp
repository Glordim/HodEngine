#include "HodApplication.hpp"

#include <HodEngine/Editor/Editor.hpp>
#include <HodEngine/Core/ArgumentParser.hpp>

_SingletonOverrideConstructor(HodApplication)
{

}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool HodApplication::Init(const hod::ArgumentParser& argumentParser)
{
#if defined(HOD_EDITOR)
	const hod::Argument* projectPathArgument = argumentParser.GetArgument('p', "ProjectPath");
	if (projectPathArgument != nullptr && projectPathArgument->_values[0] != nullptr)
	{
		std::filesystem::path projectPath(projectPathArgument->_values[0]);
		std::filesystem::current_path(projectPath.parent_path());
	}
#endif
	
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

#if defined(HOD_EDITOR)
	hod::editor::Editor::CreateInstance();
	if (hod::editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}
#else
	// Start game
#endif

	return true;
}
