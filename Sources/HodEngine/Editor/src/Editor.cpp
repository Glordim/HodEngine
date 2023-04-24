#include "Editor.h"

#include "HodEngine/Editor/src/MainBar.h"
#include "HodEngine/Editor/src/Project.h"

#include <HodEngine/ImGui/src/ImGuiManager.h>

#include "HodEngine/Editor/src/ProjectBrowser.h"
#include "HodEngine/Editor/src/AssetBrowserWindow.h"
#include "HodEngine/Editor/src/HierachyWindow.h"
#include "HodEngine/Editor/src/InspectorWindow.h"
#include "HodEngine/Editor/src/ViewportWindow.h"

#include "HodEngine/Core/Src/ArgumentParser.h"

#include "HodEngine/Application/src/GraphicApplications/DesktopApplications/DesktopApplication.h"
#include "HodEngine/Window/src/DesktopWindow/DesktopWindow.h"

#include "HodEngine/Core/Src/FileSystem.h"
#include "HodEngine/Core/Src/Stream/FileStream.h"
#include "HodEngine/Core/Src/Document/Document.h"
#include "HodEngine/Core/Src/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Src/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Src/Reflection/ReflectionMacros.h"
#include "HodEngine/Core/Src/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Src/Reflection/Properties/ReflectionPropertyVariable.h"

#include "RecentProjects.h"

namespace hod::editor
{
	_SingletonConstructor(Editor)
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
	bool Editor::Init(const core::ArgumentParser& argumentParser)
	{
		Project::CreateInstance();

		const core::Argument* argument = argumentParser.GetArgument('p', "project");
		if (argument == nullptr)
		{
			application::DesktopApplication* application = application::DesktopApplication::GetInstance();
			window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
			mainWindow->SetSize(600, 320);
			mainWindow->CenterToScreen();

			imgui::ImGuiManager::GetInstance()->OpenWindow<ProjectBrowser>();
			return true;
		}
		else
		{
			if (argument->_valueCount == 1)
			{
				return OpenProject(argument->_values[0]);
			}
		}

		return false;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Editor::OpenProject(const std::filesystem::path& path)
	{
		if (Project::GetInstance()->Open(path) == false)
		{
			return false;
		}
		AddProjectInRecentProject(path);

		AssetDatabase::CreateInstance();
		AssetDatabase::GetInstance()->Init();

		_mainBar = new MainBar();
		imgui::ImGuiManager::GetInstance()->SetMainBar(_mainBar);

		imgui::ImGuiManager::GetInstance()->CloseAllWindow();
		imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();

		application::DesktopApplication* application = application::DesktopApplication::GetInstance();
		window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
		mainWindow->Maximize();

		return true;
	}

	bool Editor::AddProjectInRecentProject(const std::filesystem::path& path) const
	{
		std::filesystem::path projectsPath = core::FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		core::Document document;
		core::DocumentReaderJson jsonReader;
		jsonReader.Read(document, projectsPath);

		RecentProjects recentProjects;
		RecentProjects::GetReflectionDescriptor()->DeserializeFromDocument(recentProjects, document.GetRootElement());

		bool alreadyExist = false;
		for (const std::string& projectPath : recentProjects._projectsPath)
		{
			if (projectPath == path.string())
			{
				alreadyExist = true;
				break;
			}
		}

		if (alreadyExist == false)
		{
			recentProjects._projectsPath.push_back(path.string());

			RecentProjects::GetReflectionDescriptor()->SerializeInDocument(recentProjects, document.GetRootElement());

			std::filesystem::create_directories(projectsPath.parent_path());
			core::DocumentWriterJson jsonWriter;
			jsonWriter.Write(document, projectsPath);
		}

		return true;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Editor::CreateProject(const std::filesystem::path& directory)
	{
		if (Project::GetInstance()->Create(directory) == true)
		{
			return OpenProject(Project::GetInstance()->GetProjectPath());
		}
		return false;
	}
}
