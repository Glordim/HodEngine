#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
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

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/Vector4CustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/WeakComponentCustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"

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

#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"
#include "HodEngine/Editor/PrefabEditor/PrefabEditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorTab.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorTab.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"
#include "HodEngine/Editor/SerializedDataEditor/SerializedDataEditorTab.hpp"

#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/ImGui/ImGuiManager.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "stb_image.h"

#include "Icons/HodIcon.png.h"
#include "Icons/folder.png.h"
#include "Icons/folder-open.png.h"
#include "Icons/landscape.png.h"
#include "Icons/prefab.png.h"
#include "Icons/SerializedData.png.h"
#include "Icons/Shader.png.h"

#include "HodEngine/Editor/MissingGameModuleModal.hpp"

#include <HodEngine/Game/BootInfo.hpp>

#undef max
#undef min

namespace hod::editor
{
	_SingletonConstructor(Editor)
	{
	}

	/// @brief 
	Editor::~Editor()
	{
		delete _floatingAssetBrowserWindow;
		imgui::ImGuiManager::GetInstance()->DestroyAllWindow();

		Project::DestroyInstance();
		AssetDatabase::DestroyInstance();

		delete _folderTexture;
		delete _folderOpenTexture;
		delete _sceneTexture;
		delete _prefabTexture;
		delete _serializedDataTexture;
		delete _shaderTexture;
		delete _checkerTexture;

		Vector2::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
		Vector4::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
		game::WeakComponentBase::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();
		WeakResourceBase::GetReflectionDescriptor().RemoveTrait<ReflectionTraitCustomPropertyDrawer>();

		game::Node2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CameraComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::TextureRendererComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::BoxCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CircleCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::CapsuleCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
		game::EdgeCollider2dComponent::GetReflectionDescriptor().RemoveTrait<ReflectionTraitComponentCustomEditor>();
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool Editor::Init(const ArgumentParser& argumentParser)
	{
		game::World::GetInstance()->DisableDrawJob();

		Vector2::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(new Vector2CustomEditor);
		Vector4::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(new Vector4CustomEditor);
		game::WeakComponentBase::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(new WeakComponentCustomEditor);
		WeakResourceBase::GetReflectionDescriptor().AddTrait<ReflectionTraitCustomPropertyDrawer>(new WeakResourceCustomEditor);

		game::Node2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new Node2dComponentCustomEditor);
		game::CameraComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new CameraComponentCustomEditor);
		game::TextureRendererComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new TextureRendererComponentCustomEditor);
		game::BoxCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new BoxCollider2dComponentCustomEditor);
		game::CircleCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new CircleCollider2dComponentCustomEditor);
		game::CapsuleCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new CapsuleCollider2dComponentCustomEditor);
		game::EdgeCollider2dComponent::GetReflectionDescriptor().AddTrait<ReflectionTraitComponentCustomEditor>(new EdgeCollider2dComponentCustomEditor);

		Project::CreateInstance();

		int x;
		int y;
		int component;
		stbi_uc* pixels;

		renderer::Texture::CreateInfo textureCreateInfo;
		textureCreateInfo._allowReadWrite = false;
		textureCreateInfo._filterMode = renderer::FilterMode::Linear;
		textureCreateInfo._wrapMode = renderer::WrapMode::Clamp;

		pixels = stbi_load_from_memory(HodIcon_png, HodIcon_png_size, &x, &y, &component, 0);
		_hodTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_hodTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(folder_png, folder_png_size, &x, &y, &component, 0);
		_folderTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_folderTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(folder_open_png, folder_open_png_size, &x, &y, &component, 0);
		_folderOpenTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_folderOpenTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(landscape_png, landscape_png_size, &x, &y, &component, 0);
		_sceneTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_sceneTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(prefab_png, prefab_png_size, &x, &y, &component, 0);
		_prefabTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_prefabTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(SerializedData_png, SerializedData_png_size, &x, &y, &component, 0);
		_serializedDataTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_serializedDataTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		pixels = stbi_load_from_memory(Shader_png, Shader_png_size, &x, &y, &component, 0);
		_shaderTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_shaderTexture->BuildBuffer(x, y, pixels, textureCreateInfo);

		static constexpr uint8_t primaryGrey = 71;
		static constexpr uint8_t secondaryGrey = 102;
		const uint8_t checkerBuffer[2 * 2 * 4] = {
			primaryGrey, primaryGrey, primaryGrey, 255, secondaryGrey, secondaryGrey, secondaryGrey, 255,
			secondaryGrey, secondaryGrey, secondaryGrey, 255, primaryGrey, primaryGrey, primaryGrey, 255,
		};
		
		textureCreateInfo._filterMode = renderer::FilterMode::Nearest;
		textureCreateInfo._wrapMode = renderer::WrapMode::Repeat;
		_checkerTexture = renderer::Renderer::GetInstance()->CreateTexture();
		_checkerTexture->BuildBuffer(2, 2, checkerBuffer, textureCreateInfo);

		_editorTabFactory.emplace("SceneImporter", [](std::shared_ptr<Asset> asset){ return new SceneEditorTab(asset); });
		_editorTabFactory.emplace("PrefabImporter", [](std::shared_ptr<Asset> asset){ return new PrefabEditorTab(asset); });
		_editorTabFactory.emplace("TextureImporter", [](std::shared_ptr<Asset> asset){ return new TextureEditorTab(asset); });
		_editorTabFactory.emplace("MaterialImporter", [](std::shared_ptr<Asset> asset){ return new MaterialEditorTab(asset); });
		_editorTabFactory.emplace("MaterialInstanceImporter", [](std::shared_ptr<Asset> asset){ return new MaterialInstanceEditorTab(asset); });
		_editorTabFactory.emplace("SerializedDataImporter", [](std::shared_ptr<Asset> asset){ return new SerializedDataEditorTab(asset); });

		_floatingAssetBrowserWindow = new AssetBrowserWindow();

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

		if (Project::GetInstance()->ReloadGameModule() == false)
		{
			return false;
		}

		AssetDatabase::GetInstance()->Init();

		imgui::ImGuiManager::GetInstance()->CloseAllWindow();

		imgui::ImGuiManager::GetInstance()->SetDrawCallback([this]()
		{
			static bool ImguiDemo = false;
			if (ImguiDemo)
			{
				ImGui::ShowDemoWindow(&ImguiDemo);
			}

			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			bool visible = ImGui::BeginMainMenuBar();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(2);
			if (visible)
			{
				ImGui::SetCursorScreenPos(ImVec2(64.0f, 0.0f));

				if (ImGui::BeginMenu("File") == true)
				{
					if (ImGui::MenuItem("Save", "CTRL+S") == true)
					{
						Editor::GetInstance()->Save();

					}

					if (ImGui::MenuItem("Save scene As", "CTRL+S") == true)
					{
						Editor::GetInstance()->SaveSceneAs();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Build") == true)
					{
						Editor::GetInstance()->Build();
					}
					if (ImGui::MenuItem("Build and Run") == true)
					{
						Editor::GetInstance()->BuildAndRun();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Quit") == true)
					{
						//application::Application::GetInstance()->Quit();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit") == true)
				{
					if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
					if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
					ImGui::Separator();
					if (ImGui::MenuItem("Cut", "CTRL+X")) {}
					if (ImGui::MenuItem("Copy", "CTRL+C")) {}
					if (ImGui::MenuItem("Paste", "CTRL+V")) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Window") == true)
				{
					if (ImGui::MenuItem("Asset Browser") == true)
					{
						imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
					}
					if (ImGui::MenuItem("Inspector") == true)
					{
						imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
					}
					if (ImGui::MenuItem("Viewport") == true)
					{
						imgui::ImGuiManager::GetInstance()->OpenWindow<ViewportWindow>();
					}
					if (ImGui::MenuItem("Hierachy") == true)
					{
						imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
					}
					if (ImGui::MenuItem("ImGui demo"))
					{
						ImguiDemo = !ImguiDemo;
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Hot Reload") == true)
				{
					Project::GetInstance()->ReloadGameModule();
				}

				_fpsCounter.OnFrame();
				char fps[64] = { '\0' };
				std::format_to_n(fps, sizeof(fps) - 1, "{} fps", (uint32_t)_fpsCounter.GetFPS());
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(fps).x);
				ImGui::BeginDisabled();
				ImGui::TextUnformatted(fps);
				ImGui::EndDisabled();

				ImGui::EndMainMenuBar();
			}

			ImVec2 logoPos = ImVec2((64.0f - 48.0f) * 0.5f, 0.0f);
			ImGui::GetForegroundDrawList()->AddImage(_hodTexture, logoPos, logoPos + ImVec2(48.0f, 48.0f));

			float statusBarHeight = 32;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
			ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight() - statusBarHeight));
			bool open = ImGui::Begin("TabBarContainer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(2);
			if (open)
			{
				if (_editorTabs.empty() == false)
				{
					ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(64.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
					bool visible = ImGui::BeginTabBar("EditorTabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_DrawSelectedOverline);
					ImGui::PopStyleVar();
					if (visible)
					{
						auto it = _editorTabs.begin();
						while (it != _editorTabs.end())
						{
							if ((*it)->Draw() == false)
							{
								delete (*it);
								_editorTabs.erase(it);
							}
							else
							{
								++it;
							}
						}
						ImGui::EndTabBar();
					}
				}
				else
				{
					if (_showFloatingAssetBrowser == false)
					{
						_showFloatingAssetBrowser = true;
						_focusFloatingAssetBrowserWindow = true;
					}
				}
			}
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - statusBarHeight));
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, statusBarHeight));

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
											ImGuiWindowFlags_NoMove | 
											ImGuiWindowFlags_NoSavedSettings | 
											ImGuiWindowFlags_NoBringToFrontOnFocus | 
											ImGuiWindowFlags_NoDocking;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			bool draw = ImGui::Begin("Bottom Menu Bar", nullptr, window_flags);
			ImGui::PopStyleVar(2);
			if (draw)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 12.0f);
				ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_Space, ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_Tooltip);
				if (ImGui::Button(ICON_MDI_FOLDER_SEARCH "  Asset Browser", ImVec2(0.0f, statusBarHeight)))
				{
					_showFloatingAssetBrowser = !_showFloatingAssetBrowser;
					if (_showFloatingAssetBrowser)
						_focusFloatingAssetBrowserWindow = true;
				}
				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
				ImGui::SameLine();
				if (ImGui::Button(ICON_MDI_TEXT_BOX "  Output Log", ImVec2(0.0f, statusBarHeight)))
				{

				}
				ImGui::PopStyleVar(3);
				ImGui::PopStyleColor();
			}
			ImGui::End();

			if (_showFloatingAssetBrowser)
			{
				_floatingAssetBrowserWindowPos = std::min(350.0f, _floatingAssetBrowserWindowPos + ImGui::GetIO().DeltaTime * 2500.0f);
			}
			else
			{
				_floatingAssetBrowserWindowPos = std::max(0.0f, _floatingAssetBrowserWindowPos - ImGui::GetIO().DeltaTime * 2500.0f);
			}
			if (_floatingAssetBrowserWindowPos > 0.0f)
			{
				ImGui::SetNextWindowPos(ImVec2(20.0f, ImGui::GetIO().DisplaySize.y - statusBarHeight - _floatingAssetBrowserWindowPos));
				ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 40.0f, 350));
				if (_focusFloatingAssetBrowserWindow)
				{
					_focusFloatingAssetBrowserWindow = false;
					ImGui::SetNextWindowFocus();
				}
				_floatingAssetBrowserWindow->Draw();
			}
		});

		OpenEditorTab<SceneEditorTab>();
/*
		imgui::ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<HierachyWindow>();
		imgui::ImGuiManager::GetInstance()->OpenWindow<InspectorWindow>();
*/
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

	/// @brief 
	/// @param asset 
	/// @return 
	EditorTab* Editor::OpenAsset(std::shared_ptr<Asset> asset)
	{
		for (EditorTab* editorTab : _editorTabs)
		{
			if (editorTab->GetAsset() == asset)
			{
				// todo select tab if docking
				return editorTab;
			}
		}
		
		auto it = _editorTabFactory.find(asset->GetMeta()._importerType);
		if (it == _editorTabFactory.end())
		{
			return nullptr; // todo message
		}

		EditorTab* editorTab = it->second(asset);
		_editorTabs.push_back(editorTab);
		return editorTab;
	}

	/// @brief 
	/// @param asset 
	void Editor::PingAsset(std::shared_ptr<Asset> asset)
	{
		_showFloatingAssetBrowser = true;
		_focusFloatingAssetBrowserWindow = true;
		_floatingAssetBrowserWindow->PingAsset(*asset.get());
	}

	/*
	/// @brief 
	void Editor::Play()
	{
		if (_playing == true)
		{
			return;
		}

		game::World* world = game::World::GetInstance();

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
	*/

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
