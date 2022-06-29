#include "Editor.h"

#include "HodEngine/Editor/src/MainBar.h"
#include "HodEngine/Editor/src/Project.h"

#include <HodEngine/ImGui/src/ImGuiManager.h>

namespace hod::editor
{
	/// @brief 
	Editor::Editor()
	{
	}

	/// @brief 
	Editor::~Editor()
	{
		delete _mainBar;
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool Editor::Init()
	{
		_mainBar = new MainBar();
		imgui::ImGuiManager::GetInstance()->SetMainBar(_mainBar);

		_assetDatabase.Init();

		return true;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Editor::CreateProject(const std::filesystem::path& directory)
	{
		std::filesystem::create_directory(directory);
		std::filesystem::path projectFilePath = directory;
		projectFilePath.append(directory.filename().string() + ".hod");

		_project = new Project(projectFilePath);
		_project->Save();

		return true;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Editor::OpenProject(const std::filesystem::path& path)
	{
		_project = new Project(path);
		_project->Load();
		return true;
	}

	/// @brief 
	/// @return 
	bool Editor::CloseProject()
	{
		delete _project;
		_project = nullptr;

		return true;
	}

	/// @brief 
	/// @return 
	Project* Editor::GetProject()
	{
		return _project;
	}

	/// @brief 
	/// @return 
	AssetDatabase& Editor::GetAssetDatabase()
	{
		return _assetDatabase;
	}

	/// @brief 
	/// @return 
	CORE::Signal<Project*>& Editor::GetOnProjectOpenedSignal()
	{
		return _onProjectOpenedSignal;
	}

	/// @brief 
	/// @return 
	CORE::Signal<Project*>& Editor::GetOnProjectClosedSignal()
	{
		return _onProjectClosedSignal;
	}
}
