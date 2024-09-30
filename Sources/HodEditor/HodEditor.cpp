#include "HodEditor.hpp"

#include <HodEngine/Editor/Editor.hpp>

#include <HodEngine/Core/ArgumentParser.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

_SingletonOverrideConstructor(HodEditor)
{

}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
bool HodEditor::Init(const hod::ArgumentParser& argumentParser)
{
	const hod::Argument* projectPathArgument = argumentParser.GetArgument('p', "ProjectPath");
	if (projectPathArgument != nullptr && projectPathArgument->_values[0] != nullptr)
	{
		std::filesystem::path projectPath(projectPathArgument->_values[0]);
		try
		{
			std::filesystem::current_path(projectPath.parent_path());
		}
		catch (const std::exception& e)
		{
			OUTPUT_ERROR(e.what());
		}		
	}
	
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

	hod::editor::Editor::CreateInstance();
	if (hod::editor::Editor::GetInstance()->Init(argumentParser) == false)
	{
		return false;
	}

	return true;
}

/// @brief 
void HodEditor::Terminate()
{
	hod::editor::Editor::DestroyInstance();

	PlatformApplication::Terminate();
}
