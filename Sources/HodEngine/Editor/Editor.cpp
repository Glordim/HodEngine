#include "HodEngine/Editor/Pch.hpp"
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

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
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

#include "HodEngine/Window/Dialog/PlatformDialog.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/SerializedDataImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/Node2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CameraComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/BoxCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CircleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CapsuleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/EdgeCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/TextureRendererComponentCustomEditor.hpp"
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/CameraComponent.hpp>
#include <HodEngine/Game/Components/TextureRendererComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp>

#include "HodEngine/Game/WeakResource.hpp"
#include "HodEngine/ImGui/ImGuiManager.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "stb_image.h"

#include "Icons/folder.png.h"
#include "Icons/folder-open.png.h"
#include "Icons/landscape.png.h"
#include "Icons/prefab.png.h"
#include "Icons/SerializedData.png.h"

#include "HodEngine/Editor/MissingGameModuleModal.hpp"

#include <HodEngine/Game/BootInfo.hpp>

namespace hod::editor
{
	_SingletonConstructor(Editor)
	{
	}

	/// @brief 
	Editor::~Editor()
	{
		imgui::ImGuiManager::GetInstance()->DestroyAllWindow();

		game::WeakComponentMapping::Clear(); // must be clear before GameModule unload

		Project::DestroyInstance();
		AssetDatabase::DestroyInstance();
		delete _mainBar;

		delete _folderTexture;
		delete _folderOpenTexture;
		delete _sceneTexture;
		delete _prefabTexture;
		delete _serializedDataTexture;
		delete _checkerTexture;

		Vector2::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
		game::WeakResourceBase::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
		TextureImporterSettings::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitImporterCustomEditor>();

		game::Node2dComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CameraComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::TextureRendererComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::BoxCollider2dComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CircleCollider2dComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CapsuleCollider2dComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::EdgeCollider2dComponent::GetReflectionDescriptor()->RemoveTrait<ReflectionTraitComponentCustomEditor>();
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool Editor::Init(const ArgumentParser& argumentParser)
	{
		game::World::GetInstance()->DisableDrawJob();

		Vector2::GetReflectionDescriptor()->AddTrait<ReflectionTraitCustomPropertyDrawer>(new Vector2CustomEditor);
		game::WeakResourceBase::GetReflectionDescriptor()->AddTrait<ReflectionTraitCustomPropertyDrawer>(new WeakResourceCustomEditor);
		TextureImporterSettings::GetReflectionDescriptor()->AddTrait<ReflectionTraitImporterCustomEditor>(new TextureImporterCustomEditor);
		SerializedDataImporterSettings::GetReflectionDescriptor()->AddTrait<ReflectionTraitImporterCustomEditor>(new SerializedDataImporterCustomEditor);

		game::Node2dComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new Node2dComponentCustomEditor);
		game::CameraComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new CameraComponentCustomEditor);
		game::TextureRendererComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new TextureRendererComponentCustomEditor);
		game::BoxCollider2dComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new BoxCollider2dComponentCustomEditor);
		game::CircleCollider2dComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new CircleCollider2dComponentCustomEditor);
		game::CapsuleCollider2dComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new CapsuleCollider2dComponentCustomEditor);
		game::EdgeCollider2dComponent::GetReflectionDescriptor()->AddTrait<ReflectionTraitComponentCustomEditor>(new EdgeCollider2dComponentCustomEditor);

		Project::CreateInstance();

		int x;
		int y;
		int component;
		stbi_uc* pixels = stbi_load_from_memory(folder_png, folder_png_size, &x, &y, &component, 0);

		_folderTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_folderTexture->BuildBuffer(x, y, pixels, renderer::Texture::CreateInfo());

		pixels = stbi_load_from_memory(folder_open_png, folder_open_png_size, &x, &y, &component, 0);

		_folderOpenTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_folderOpenTexture->BuildBuffer(x, y, pixels, renderer::Texture::CreateInfo());

		pixels = stbi_load_from_memory(landscape_png, landscape_png_size, &x, &y, &component, 0);

		_sceneTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_sceneTexture->BuildBuffer(x, y, pixels, renderer::Texture::CreateInfo());

		pixels = stbi_load_from_memory(prefab_png, prefab_png_size, &x, &y, &component, 0);

		_prefabTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_prefabTexture->BuildBuffer(x, y, pixels, renderer::Texture::CreateInfo());

		pixels = stbi_load_from_memory(SerializedData_png, SerializedData_png_size, &x, &y, &component, 0);

		_serializedDataTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_serializedDataTexture->BuildBuffer(x, y, pixels, renderer::Texture::CreateInfo());

		static constexpr uint8_t primaryGrey = 71;
		static constexpr uint8_t secondaryGrey = 102;
		const uint8_t checkerBuffer[2 * 2 * 4] = {
			primaryGrey, primaryGrey, primaryGrey, 255, secondaryGrey, secondaryGrey, secondaryGrey, 255,
			secondaryGrey, secondaryGrey, secondaryGrey, 255, primaryGrey, primaryGrey, primaryGrey, 255,
		};
		renderer::Texture::CreateInfo textureCreateInfo;
		textureCreateInfo._filterMode = renderer::FilterMode::Nearest;
		textureCreateInfo._wrapMode = renderer::WrapMode::Repeat;
		_checkerTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_checkerTexture->BuildBuffer(2, 2, checkerBuffer, textureCreateInfo);

		const hod::Argument* projectPathArgument = argumentParser.GetArgument('p', "ProjectPath");
		if (projectPathArgument == nullptr || projectPathArgument->_values[0] == nullptr)
		{
			window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(imgui::ImGuiManager::GetInstance()->GetMainWindow());
			mainWindow->SetSize(800, 320);
			mainWindow->CenterToScreen();
			mainWindow->SetVisible(true);

			imgui::ImGuiManager::GetInstance()->OpenWindow<ProjectBrowser>();
			return true;
		}
		else
		{
			return OpenProject(projectPathArgument->_values[0]);
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

		if (Project::GetInstance()->HasGameModule() == false)
		{
			imgui::ImGuiManager::GetInstance()->CloseAllWindow();
			imgui::ImGuiManager::GetInstance()->OpenWindow<MissingGameModuleModal>();
			return true;
		}
		else
		{
			return LoadEditor();
		}
	}

	bool Editor::LoadEditor()
	{
		AssetDatabase::CreateInstance();

		_mainBar = new MainBar();
		imgui::ImGuiManager::GetInstance()->SetMainBar(_mainBar);

		if (Project::GetInstance()->ReloadGameModule() == false)
		{
			return false;
		}

		AssetDatabase::GetInstance()->Init();

		imgui::ImGuiManager::GetInstance()->CloseAllWindow();
		imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
		//imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();

		window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(imgui::ImGuiManager::GetInstance()->GetMainWindow());
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
	void Editor::SetEntitySelection(std::shared_ptr<game::Entity> selection)
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

		std::shared_ptr<Asset> asset = selection->_asset;
		if (asset != nullptr)
		{
			ReflectionDescriptor* reflectionDescriptor = asset->GetMeta()._importerSettings->GetReflectionDescriptorV();
			ReflectionTraitImporterCustomEditor* componentCustomEditorTrait = reflectionDescriptor->FindTrait<ReflectionTraitImporterCustomEditor>();
			if (componentCustomEditorTrait != nullptr)
			{
				componentCustomEditorTrait->GetCustomEditor()->OnInit(asset);
			}
		}
	}

	/// @brief 
	/// @return 
	std::shared_ptr<game::Entity> Editor::GetEntitySelection() const
	{
		return _entitySelection.lock();
	}

	/// @brief 
	/// @return 
	const AssetDatabase::FileSystemMapping* Editor::GetAssetSelection() const
	{
		return _assetSelection;
	}

	/// @brief 
	/// @return 
	ViewportWindow* Editor::GetCurrentViewport() const
	{
		return _currentViewport;
	}

	/// @brief 
	/// @param viewportWindow 
	void Editor::SetCurrentViewport(ViewportWindow* viewportWindow)
	{
		_currentViewport = viewportWindow;
	}

	/// @brief 
	/// @return 
	bool Editor::Save()
	{
			/*
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
			*/

		return AssetDatabase::GetInstance()->Save();
	}

	/// @brief 
	/// @return 
	bool Editor::SaveSceneAs()
	{
		/*
		std::filesystem::path saveLocation = window::GetSaveFileDialog("Hod Asset", "asset", Project::GetInstance()->GetAssetDirPath());
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
		*/
		return true;
	}

	void Editor::OpenAsset(std::shared_ptr<Asset> asset)
	{
		bool alreadyExist = false;
		std::vector<imgui::Window*> viewportWindows = imgui::ImGuiManager::GetInstance()->FindWindows<ViewportWindow>();
		for (imgui::Window* window : viewportWindows)
		{
			ViewportWindow* viewportWindows = static_cast<ViewportWindow*>(window);

			if (viewportWindows->GetAsset() == asset)
			{
				// todo select tab if docking
				alreadyExist = true;
				break;
			}
		}
		if (alreadyExist == false)
		{
			imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>(asset);
			// todo with others ViewportWindows
		}
		/*
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
		world->AddScene(scene);
		//world->LoadFromDocument(scene->GetDocument().GetRootNode());

		_currentScene = &asset;
		*/
	}

	/// @brief 
	void Editor::Play()
	{
		if (_playing == true)
		{
			return;
		}

		game::World* world = game::World::GetInstance();
		/*
		_playedWorldDocument.GetRootNode().Clear();
		if (world->SaveToDocument(_playedWorldDocument.GetRootNode()) == false)
		{
			return;
		}
		*/
		world->SetEditorPlaying(true);

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

		game::World* world = game::World::GetInstance();
		world->SetEditorPlaying(_playing);
		world->SetEditorPaused(_paused);
		world->Clear();
		_currentViewport->ReloadScene();
		
		/*
		world->Clear();
		if (world->LoadFromDocument(_playedWorldDocument.GetRootNode()) == false)
		{
			return;
		}
		*/
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
		Pause();
		game::World::GetInstance()->EditorNextFrame();
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

	/// @brief 
	/// @return 
	Asset* Editor::GetCurrentScene() const
	{
		return _currentScene;
	}

	/// @brief 
	void Editor::MarkCurrentSceneAsDirty()
	{
		if (_currentViewport && _playing == false)
		{
			_currentViewport->MarkCurrentSceneAsDirty();
		}
	}

	/// @brief 
	void Editor::Build()
	{
		try
		{
			std::filesystem::path buildPath = Project::GetInstance()->GetBuildsDirPath() / "Latest";
			std::filesystem::create_directories(buildPath);

			game::BootInfo bootInfo;
			bootInfo._startupScene = Project::GetInstance()->GetStartupScene();
			bootInfo._gameModule = Project::GetInstance()->GetName();

			Document bootDocument;
			Serializer::Serialize(bootInfo, bootDocument.GetRootNode());

			DocumentWriterJson writer;
			writer.Write(bootDocument, buildPath / "Boot.json");

			std::filesystem::copy(Project::GetInstance()->GetGameModulePath(), buildPath / Project::GetInstance()->GetGameModulePath().filename(), std::filesystem::copy_options::overwrite_existing);
			// todo copy hodapplication.exe

			std::filesystem::path dataDirPath = buildPath / "Datas";
			std::filesystem::create_directories(dataDirPath);

			for (const auto& entry : std::filesystem::directory_iterator(Project::GetInstance()->GetResourceDirPath()))
			{
				const std::filesystem::path& source = entry.path();
				if (std::filesystem::is_regular_file(source))
				{
					std::filesystem::path destination = dataDirPath / source.filename();
					std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
				}
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_ERROR("Build error : {}", e.what());
			return;
		}
	}

	/// @brief 
	void Editor::BuildAndRun()
	{
		Build();
		// todo run
	}
}
