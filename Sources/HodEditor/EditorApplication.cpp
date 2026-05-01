#include "EditorApplication.hpp"

#include <HodEngine/Editor/Editor.hpp>

#include <HodEngine/Core/ArgumentParser.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <filesystem> // todo remove

#include <HodEngine/Application/InitGuard.hpp>

namespace hod
{
	bool EditorApplication::RunInternal()
	{
		InitGuard initGuard;

		if (!initGuard.Push([this]{ return InitCore(); },        [this]{ TerminateCore(); }))        return false;
		if (!initGuard.Push([this]{ return InitGameSystems(); }, [this]{ TerminateGameSystems(); })) return false;
		if (!initGuard.Push([this]{ return InitPhysics(); },     [this]{ TerminatePhysics(); }))     return false;
		if (!initGuard.Push([this]{ return InitAudio(); },       [this]{ TerminateAudio(); }))       return false;
		if (!initGuard.Push([this]{ return InitWindow(); },      [this]{ TerminateWindow(); }))      return false;
		if (!initGuard.Push([this]{ return InitInput(); },       [this]{ TerminateInput(); }))       return false;
		if (!initGuard.Push([this]{ return InitRenderer(); },    [this]{ TerminateRenderer(); }))    return false;
		if (!initGuard.Push([this]{ return InitImGui(); },       [this]{ TerminateImGui(); }))       return false;
		if (!initGuard.Push([this]{ return InitGame(); },        [this]{ TerminateGame(); }))        return false;
		if (!initGuard.Push([this]{ return InitEditor(); },      [this]{ TerminateEditor(); }))      return false;

		return RunGraphicLoop();
	}

	bool EditorApplication::InitEditor()
	{
		ArgumentParser argumentParser(0, nullptr); // todo pass argc/argv

		const hod::Argument* projectPathArgument = argumentParser.GetArgument('p', "ProjectPath");
		if (projectPathArgument != nullptr && projectPathArgument->_values[0] != nullptr)
		{
			hod::Path projectPath(projectPathArgument->_values[0]);
			try
			{
				std::filesystem::current_path(projectPath.ParentPath().GetString().CStr());
			}
			catch (const std::exception& e)
			{
				OUTPUT_ERROR(e.what());
			}
		}

		hod::Editor::CreateInstance();
		if (hod::Editor::GetInstance()->Init(argumentParser) == false)
		{
			return false;
		}

		return true;
	}

	bool EditorApplication::TerminateEditor()
	{
		Editor::DestroyInstance();
		return true;
	}
}
