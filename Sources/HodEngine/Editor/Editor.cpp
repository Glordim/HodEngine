#include "HodEngine/Editor/Editor.h"

#include "HodEngine/Editor/MainBar.h"
#include "HodEngine/Editor/Project.h"

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/ProjectBrowser.h"
#include "HodEngine/Editor/AssetBrowserWindow.h"
#include "HodEngine/Editor/HierachyWindow.h"
#include "HodEngine/Editor/InspectorWindow.h"
#include "HodEngine/Editor/ViewportWindow.h"

#include "HodEngine/Core/ArgumentParser.h"

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.h"
#include "HodEngine/Window/Desktop/DesktopWindow.h"

#include "HodEngine/Core/FileSystem.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Reflection/ReflectionMacros.h"
#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

#include "HodEngine/Editor/RecentProjects.h"
#include "HodEngine/Editor/Asset.h"
#include "HodEngine/Game/Scene.h"
#include "HodEngine/Game/World.h"

#include "HodEngine/Application/PlatformDialog.h"

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
	bool Editor::Init(const ArgumentParser& argumentParser)
	{
		Project::CreateInstance();

		const Argument* argument = argumentParser.GetArgument('p', "project");
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
		RecentProjects recentProjects;

		std::filesystem::path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		// Read and deserialize existing file
		{
			Document document;
			DocumentReaderJson jsonReader;
			if (jsonReader.Read(document, projectsPath) == true)
			{
				RecentProjects::GetReflectionDescriptor()->DeserializeFromDocument(recentProjects, document.GetRootNode());
			}
		}

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

			Document writeDocument;
			RecentProjects::GetReflectionDescriptor()->SerializeInDocument(recentProjects, writeDocument.GetRootNode());

			std::filesystem::create_directories(projectsPath.parent_path());
			DocumentWriterJson jsonWriter;
			jsonWriter.Write(writeDocument, projectsPath);
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

	/// @brief 
	/// @param asset 
	void Editor::SetEntitySelection(std::weak_ptr<game::Entity> selection)
	{
		_assetSelection = nullptr;
		_entitySelection = selection;
	}

	/// @brief 
	/// @param asset 
	void Editor::SetAssetSelection(const AssetDatabase::FileSystemMapping* selection)
	{
		_assetSelection = selection;
		_entitySelection.reset();
	}

	/// @brief 
	/// @return 
	const std::weak_ptr<game::Entity>& Editor::GetEntitySelection() const
	{
		return _entitySelection;
	}

	/// @brief 
	/// @return 
	const AssetDatabase::FileSystemMapping* Editor::GetAssetSelection() const
	{
		return _assetSelection;
	}

	/// @brief 
	/// @return 
	bool Editor::Save()
	{
		return AssetDatabase::GetInstance()->Save();
	}

	/// @brief 
	/// @return 
	bool Editor::SaveSceneAs()
	{
		std::filesystem::path saveLocation = application::dialog::GetSaveFileDialog("Hod Asset", "asset", Project::GetInstance()->GetAssetDirPath());
		if (saveLocation.empty() == true)
		{
			return false;
		}

		Document worldDocument;

		game::World* world = game::World::GetInstance();
		if (world->SaveToDocument(worldDocument.GetRootNode()) == false)
		{
			return false;
		}

		game::Scene scene;
		if (scene.DeserializeFromDocument(worldDocument.GetRootNode()) == false)
		{
			return false;
		}

		AssetDatabase::GetInstance()->CreateAsset(&scene, scene.GetReflectionDescriptorV(), saveLocation);
		// Todo return value is unique path no way if failure ?
		if (true == false)
		{
			return false;
		}

		// todo open it

		return true;
	}

	void Editor::OpenAsset(Asset& asset)
	{
		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, asset.GetPath()) == false)
		{
			return; // todo message + bool
		}

		game::Scene* scene = new game::Scene();
		if (scene->GetReflectionDescriptorV()->DeserializeFromDocument((void*)scene, document.GetRootNode()) == false)
		{
			return; // todo message + bool
		}

		game::World* world = game::World::GetInstance();
		world->LoadFromDocument(scene->GetDocument().GetRootNode());
	}
}
