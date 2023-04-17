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
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

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
	bool Editor::Init(const core::ArgumentParser& argumentParser)
	{
		const core::Argument* argument = argumentParser.GetArgument('p', "project");
		if (argument == nullptr)
		{
			application::DesktopApplication* application = application::DesktopApplication::GetInstance();
			window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
			mainWindow->SetSize(600, 320);
			mainWindow->CenterToScreen();

			imgui::ImGuiManager::GetInstance()->OpenWindow<ProjectBrowser>();
		}
		else
		{
			OpenProject(argument->_values[0]); // todo verif if value
		}

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

		OnProjectLoaded.Emit(_project);

		return true;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Editor::OpenProject(const std::filesystem::path& path)
	{
		std::filesystem::path projectsPath = core::FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		rapidjson::Document document;
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		rapidjson::Value projects(rapidjson::kArrayType);
		projects.SetArray();
		bool alreadyExist = false;
		char* buffer = nullptr;

		std::ifstream file;
		file.open(projectsPath, std::ios::in);
		if (file.is_open() == true)
		{
			file.seekg(0, std::ios::end);

			int size = (int)file.tellg();
			buffer = new char[size + 1];
			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();
			buffer[size] = '\0';
			for (int i = size - 1; i >= 0; --i)
			{
				if (buffer[i] == '\0' || buffer[i] == 4)
				{
					buffer[i] = '\0';
					break;
				}
			}

			document.Parse(buffer);
			projects = document["Projects"].GetArray();

			for (const auto& project : projects.GetArray())
			{
				if (project.GetString() == path.string())
				{
					alreadyExist = true;
					break;
				}
			}

			if (alreadyExist == false)
			{
				rapidjson::Value strVal;
				strVal.SetString(path.string().c_str(), path.string().size(), allocator);
				projects.PushBack(strVal, allocator);
			}
		}
		else
		{
			document.SetObject();
			rapidjson::Value strVal;
			strVal.SetString(path.string().c_str(), path.string().size(), allocator);
			projects.PushBack(strVal, allocator);
			document.AddMember("Projects", projects, allocator);
		}

		if (alreadyExist == false)
		{
			rapidjson::StringBuffer buf;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
			document.Accept(writer);

			const char* json = buf.GetString();
			size_t jsonSize = buf.GetSize();

			std::filesystem::create_directories(projectsPath.parent_path());

			std::ofstream file;
			file.open(projectsPath, std::ios::out);
			if (file.is_open() == true)
			{
				file.seekp(0, std::ios::beg);
				file.write(json, jsonSize);
				file.flush();
				file.close();
			}
		}
		if (buffer != nullptr)
		{
			delete[] buffer;
		}

		imgui::ImGuiManager::GetInstance()->CloseAllWindow();
		application::DesktopApplication* application = application::DesktopApplication::GetInstance();
		window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
		mainWindow->Maximize();

		_mainBar = new MainBar();
		imgui::ImGuiManager::GetInstance()->SetMainBar(_mainBar);

		_assetDatabase.Init();

		imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();

		_project = new Project(path);
		_project->Load();

		OnProjectLoaded.Emit(_project);

		return true;
	}

	/// @brief 
	/// @return 
	bool Editor::CloseProject()
	{
		OnProjectClosed.Emit(_project);

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
}
