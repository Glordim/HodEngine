#include "HodEngine/Editor/Editor.hpp"

#include "HodEngine/Editor/MainBar.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include "HodEngine/Core/ArgumentParser.hpp"

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"
#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

#include "HodEngine/Editor/RecentProjects.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Application/PlatformDialog.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/Vector2CustomEditor.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

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
		Vector2::GetReflectionDescriptor()->AddTrait<ReflectionTraitCustomPropertyDrawer>(new Vector2CustomEditor);
		TextureImporterSettings::GetReflectionDescriptor()->AddTrait<ReflectionTraitImporterCustomEditor>(new TextureImporterCustomEditor);

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
				Serializer::Deserialize(recentProjects, document.GetRootNode());
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
			Serializer::Serialize(recentProjects, writeDocument.GetRootNode());

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
		if (_currentScene == nullptr)
		{
			if (SaveSceneAs() == false)
			{
				return false;
			}
		}
		else
		{
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

			_currentScene->Save(&scene, scene.GetReflectionDescriptorV());
		}

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
		if (Serializer::Deserialize(scene, document.GetRootNode()) == false)
		{
			return; // todo message + bool
		}

		game::World* world = game::World::GetInstance();
		world->Clear();
		world->LoadFromDocument(scene->GetDocument().GetRootNode());

		_currentScene = &asset;
	}

	/// @brief 
	void Editor::Play()
	{
		if (_playing == true)
		{
			return;
		}

		game::World::GetInstance()->SetEditorPlaying(true);

		_playing = true;
	}

	/// @brief 
	void Editor::Stop()
	{
		if (_playing == false)
		{
			return;
		}

		_playing = false;
		_paused = false;

		game::World::GetInstance()->SetEditorPlaying(_playing);
		game::World::GetInstance()->SetEditorPaused(_paused);

		OpenAsset(*_currentScene);
	}
	
	/// @brief 
	void Editor::Pause()
	{
		if (_paused == true)
		{
			return;
		}

		_paused = true;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void Editor::Resume()
	{
		if (_paused == false)
		{
			return;
		}

		_paused = false;

		game::World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief 
	void Editor::PlayNextFrame()
	{
		
	}

	/// @brief 
	/// @return 
	bool Editor::IsPlaying() const
	{
		return _playing;
	}

	/// @brief 
	/// @return 
	bool Editor::IsPaused() const
	{
		return _paused;
	}
}
