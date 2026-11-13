#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Editor/Cooker/CookJob.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include <HodEngine/Core/ModuleHost.hpp>
#include "HodEngine/Editor/EditorModule.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/GameSystems/Job/JobScheduler.hpp"
#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "TaskTracker/TaskTracker.hpp"
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/ProjectBrowser.hpp"
#include "HodEngine/Editor/ProjectSettingsWindow.hpp"
#include "HodEngine/Editor/Plugins/PluginsWindow.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTrackerWindow.hpp"
#include "HodEngine/Editor/TitleBar.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlChangesWindow.hpp"

#include "HodEngine/Core/ArgumentParser.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"

#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Editor/WindowFactory.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetType.hpp"
#include "HodEngine/Editor/CMakeHelper.hpp"
#include "HodEngine/Editor/Importer/ImportJob.hpp"
#include "HodEngine/Editor/RecentProjects.hpp"
#include "HodEngine/Game/Scene.hpp"
#include <HodEngine/Core/RuntimeModule.hpp>

#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/Texture.hpp"
#include <fmt/format.h>
#include <stb_image.h>

#include "Icons/folder.png.hpp"
#include "Icons/HodIcon.png.hpp"
#include "Icons/landscape.png.hpp"
#include "Icons/prefab.png.hpp"
#include "Icons/Material.png.hpp"
#include "Icons/Font.png.hpp"
#include "Icons/Audio.png.hpp"
#include "Icons/DataItem.png.hpp"
#include "Icons/DataTable.png.hpp"

#include "HodEngine/Editor/MissingProjectModuleModal.hpp"

#include <filesystem> // todo remove
#include <string>

#include "portable-file-dialogs.h"

#include "HodEngine/Core/StaticArray.hpp"

#include <utility>

#undef max
#undef min

namespace hod::inline editor
{
	_SingletonConstructor(Editor)
	: _onProjectModulesReloadedSlot([]() { AssetBrowserWindow::PopulateContextualMenuActions(); })
	{
	}

	/// @brief
	Editor::~Editor()
	{
		for (EditorTab* editorTab : _editorTabs)
		{
			DefaultAllocator::GetInstance().Delete(editorTab);
		}
		_editorTabs.Clear();

		// After the tabs (they release their checkouts) and before the editor modules (the providers go with them): runs what's queued
		VersionControl::GetInstance()->OnProjectClosed();
		Project::GetInstance()->UnloadProjectModules(); // Before the engine's editor modules: loaded after them
		UnloadEditorModules();
		VersionControl::DestroyInstance();

		DefaultAllocator::GetInstance().Delete(_floatingAssetBrowserWindow);
		DefaultAllocator::GetInstance().Delete(_floatingTaskTrackerWindow);
		ImGuiManager::GetInstance()->DestroyAllWindow();

		Project::DestroyInstance();
		AssetDatabase::DestroyInstance();
		WindowFactory::DestroyInstance();

		UnloadRuntimeModules(); // Last: their components and world systems may be used until then

		// Once everything is released (the copy of the game module, the libraries): the new editor reloads and rebuilds them
		if (_restartRequested)
		{
			Process::Desc desc;
			desc._program = FileSystem::GetExecutablePath().GetString();
			desc._arguments = {"-p", _restartProjectPath.GetString()};
			desc._workingDirectory = _restartProjectPath.ParentPath();
			if (Process::Launch(desc) == false)
			{
				OUTPUT_ERROR("The editor couldn't be started again on {}", _restartProjectPath);
			}
		}

		DefaultAllocator::GetInstance().Delete(_hodTexture);
		DefaultAllocator::GetInstance().Delete(_folderTexture);
		DefaultAllocator::GetInstance().Delete(_sceneTexture);
		DefaultAllocator::GetInstance().Delete(_prefabTexture);
		DefaultAllocator::GetInstance().Delete(_materialTexture);
		DefaultAllocator::GetInstance().Delete(_fontTexture);
		DefaultAllocator::GetInstance().Delete(_audioTexture);
		DefaultAllocator::GetInstance().Delete(_dataItemTexture);
		DefaultAllocator::GetInstance().Delete(_dataTableTexture);
		DefaultAllocator::GetInstance().Delete(_checkerTexture);

		DefaultAllocator::GetInstance().Delete(_taskTracker);
	}

	/// @brief
	/// @param window
	/// @return
	bool Editor::Init(const ArgumentParser& argumentParser)
	{
		_taskTracker = DefaultAllocator::GetInstance().New<TaskTracker>();

		// Every editor screen (project browser, missing game module, editor) draws its own title bar, see TitleBar
		static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow())->SetCustomTitleBar(true);

		Project::CreateInstance();
		Project::GetInstance()->GetOnModulesReloadedEvent().Connect(_onProjectModulesReloadedSlot);

		WindowFactory::CreateInstance();
		AssetDatabase::CreateInstance(); // Before the editor modules (loaded with the project, see LoadEditor): they register their asset types into it
		VersionControl::CreateInstance(); // Same for their version control providers

		int      x;
		int      y;
		int      component;
		stbi_uc* pixels;

		// An R8G8B8A8 texture of the given pixels
		auto createTexture = [](const uint8_t* texturePixels, int width, int height)
		{
			Texture::CreateInfo createInfo;
			createInfo._width = (uint32_t)width;
			createInfo._height = (uint32_t)height;

			Texture* texture = RhiDevice::GetInstance()->CreateTexture();
			texture->Build(createInfo, texturePixels);
			return texture;
		};

		pixels = stbi_load_from_memory(HodIcon_png, HodIcon_png_size, &x, &y, &component, 0);
		_hodTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(folder_png, folder_png_size, &x, &y, &component, 0);
		_folderTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(landscape_png, landscape_png_size, &x, &y, &component, 0);
		_sceneTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(prefab_png, prefab_png_size, &x, &y, &component, 0);
		_prefabTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(Material_png, Material_png_size, &x, &y, &component, 0);
		_materialTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(Font_png, Font_png_size, &x, &y, &component, 0);
		_fontTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(Audio_png, Audio_png_size, &x, &y, &component, 0);
		_audioTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(DataItem_png, DataItem_png_size, &x, &y, &component, 0);
		_dataItemTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		pixels = stbi_load_from_memory(DataTable_png, DataTable_png_size, &x, &y, &component, 0);
		_dataTableTexture = createTexture(pixels, x, y);
		stbi_image_free(pixels);

		static constexpr uint8_t primaryGrey = CheckerPrimaryGrey;
		static constexpr uint8_t secondaryGrey = CheckerSecondaryGrey;
		const uint8_t            checkerBuffer[2 * 2 * 4] = {
            primaryGrey,   primaryGrey,   primaryGrey,   255, secondaryGrey, secondaryGrey, secondaryGrey, 255,
            secondaryGrey, secondaryGrey, secondaryGrey, 255, primaryGrey,   primaryGrey,   primaryGrey,   255,
        };

		Sampler::CreateInfo checkerSamplerCreateInfo;
		checkerSamplerCreateInfo._filterMode = FilterMode::Nearest;
		checkerSamplerCreateInfo._wrapMode = WrapMode::Repeat;
		_checkerTexture = createTexture(checkerBuffer, 2, 2);
		_checkerTexture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(checkerSamplerCreateInfo));

		_floatingAssetBrowserWindow = DefaultAllocator::GetInstance().New<AssetBrowserWindow>();
		_floatingAssetBrowserWindow->SetFloating();
		_floatingTaskTrackerWindow = DefaultAllocator::GetInstance().New<TaskTrackerWindow>();

		const hod::Argument* projectPathArgument = argumentParser.GetArgument('p', "ProjectPath");
		if (projectPathArgument == nullptr || projectPathArgument->_values[0] == nullptr)
		{
			DesktopWindow* mainWindow = static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow());
			mainWindow->SetSize(800, 320);
			mainWindow->CenterToScreen();

			ImGuiManager::GetInstance()->OpenWindow<ProjectBrowser>();
			return true;
		}
		else
		{
			return OpenProject(projectPathArgument->_values[0]);
		}

		return false;
	}

	/// @brief The startup of each plugin's runtime modules (in the plugins' order: dependencies first; a plugin's base then its Components
	/// layer), the project settings loaded already (Project::Open). A plugin that fails (and the ones needing it) is left out, with an
	/// error: the editor still opens
	/// @return
	bool Editor::LoadRuntimeModules()
	{
		_runtimeModuleHost = DefaultAllocator::GetInstance().New<ModuleHost>();

		auto dependsOnFailed = [this](const PluginManifest& manifest)
		{
			for (const String& dependency : manifest._dependencies)
			{
				for (const String& failedPlugin : _failedPlugins)
				{
					if (failedPlugin == dependency)
					{
						return true;
					}
				}
			}
			return false;
		};

		for (const PluginGraph::Plugin& plugin : Project::GetInstance()->GetPluginGraph().GetPlugins())
		{
			const PluginManifest& manifest = plugin._manifest;
			if (plugin._enabled == false || manifest._runtimeModules.Empty())
			{
				continue;
			}
			if (dependsOnFailed(manifest))
			{
				OUTPUT_ERROR("Plugin '{}' left out: it needs a plugin that failed", manifest._name);
				_failedPlugins.PushBack(manifest._name);
				continue;
			}

			// All of its modules found before any is started: a plugin is in, or out
			Vector<DynamicLibrary*>       libraries;
			Vector<ModuleStartup>   startups;
			for (const String& runtimeModule : manifest._runtimeModules)
			{
				DynamicLibrary* library = DefaultAllocator::GetInstance().New<DynamicLibrary>();
				library->Init(FileSystem::GetExecutablePath().ParentPath() / runtimeModule, false);
				ModuleStartup startup = library->Load() ? library->LoadFunction<ModuleStartup>(HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME) : nullptr;
				libraries.PushBack(library);
				if (startup == nullptr)
				{
					OUTPUT_ERROR("Plugin '{}' left out: no " HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME " in its module {} (see HOD_STARTUP_RUNTIME_MODULE)", manifest._name, runtimeModule);
					break;
				}
				startups.PushBack(startup);
			}
			if (startups.Size() != manifest._runtimeModules.Size())
			{
				for (auto it = libraries.RBegin(); it != libraries.REnd(); ++it)
				{
					DefaultAllocator::GetInstance().Delete(*it);
				}
				_failedPlugins.PushBack(manifest._name);
				continue;
			}

			uint32_t started = 0;
			while (started < startups.Size() && _runtimeModuleHost->Start(manifest._runtimeModules[started], startups[started]))
			{
				++started;
			}
			if (started != startups.Size())
			{
				// The libraries it started already undone (its base when its Components layer failed), the ones needing it left out
				OUTPUT_ERROR("Plugin '{}' left out: its module {} failed to start", manifest._name, manifest._runtimeModules[started]);
				_runtimeModuleHost->StopLast(started);
				for (auto it = libraries.RBegin(); it != libraries.REnd(); ++it)
				{
					DefaultAllocator::GetInstance().Delete(*it);
				}
				_failedPlugins.PushBack(manifest._name);
				continue;
			}
			for (DynamicLibrary* library : libraries)
			{
				_runtimeModuleLibraries.PushBack(library);
			}
		}
		return true;
	}

	/// @brief
	void Editor::Restart()
	{
		_restartRequested = true;
		_restartProjectPath = Project::GetInstance()->GetProjectPath();
		static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow())->Close();
	}

	/// @brief
	/// @return
	bool Editor::HasDirtyTabs() const
	{
		for (const EditorTab* editorTab : _editorTabs)
		{
			if (editorTab->IsDirty())
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param name
	/// @return
	bool Editor::HasPluginFailed(const String& name) const
	{
		for (const String& failedPlugin : _failedPlugins)
		{
			if (failedPlugin == name)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	void Editor::UnloadRuntimeModules()
	{
		if (_runtimeModuleHost != nullptr)
		{
			DefaultAllocator::GetInstance().Delete(_runtimeModuleHost); // Stops them, in the reverse order
			_runtimeModuleHost = nullptr;
		}
		for (auto it = _runtimeModuleLibraries.RBegin(); it != _runtimeModuleLibraries.REnd(); ++it)
		{
			(*it)->Unload();
			DefaultAllocator::GetInstance().Delete(*it);
		}
		_runtimeModuleLibraries.Clear();
	}

	/// @brief
	/// @return
	bool Editor::LoadEditorModules()
	{
		for (const PluginGraph::Plugin& plugin : Project::GetInstance()->GetPluginGraph().GetPlugins())
		{
			const PluginManifest& manifest = plugin._manifest;
			if (plugin._enabled == false || manifest._editorModule.Empty())
			{
				continue;
			}
			bool failed = false;
			for (const String& failedPlugin : _failedPlugins)
			{
				if (failedPlugin == manifest._name)
				{
					failed = true;
					break;
				}
			}
			if (failed)
			{
				continue;
			}

			DynamicLibrary* library = DefaultAllocator::GetInstance().New<DynamicLibrary>();
			library->Init(FileSystem::GetExecutablePath().ParentPath() / manifest._editorModule, false);
			if (library->Load() == false)
			{
				OUTPUT_ERROR("Plugin '{}': its editor module {} can't be loaded", manifest._name, manifest._editorModule);
				DefaultAllocator::GetInstance().Delete(library);
				if (manifest._required)
				{
					return false;
				}
				continue;
			}

			ModuleStartup startup = library->LoadFunction<ModuleStartup>(HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME);
			if (startup == nullptr)
			{
				OUTPUT_ERROR("Plugin '{}': no " HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME " in its editor module {} (see HOD_STARTUP_EDITOR_MODULE)", manifest._name, manifest._editorModule);
				DefaultAllocator::GetInstance().Delete(library);
				if (manifest._required)
				{
					return false;
				}
				continue;
			}
			if (_editorModuleHost.Start(manifest._editorModule, startup) == false)
			{
				DefaultAllocator::GetInstance().Delete(library); // What it registered before failing undone already, while its code was loaded
				if (manifest._required)
				{
					return false;
				}
				continue;
			}
			_editorModuleLibraries.PushBack(library);
		}
		return true;
	}

	/// @brief What the editor modules registered, while their code is still loaded
	/// @return
	bool Editor::UnloadEditorModules()
	{
		// The tabs of the assets first, all of them (their asset types all go): closing a tab can use the asset types of other editor modules
		uint32_t index = 0;
		while (index < _editorTabs.Size())
		{
			if (_editorTabs[index]->GetAsset() != nullptr)
			{
				DefaultAllocator::GetInstance().Delete(_editorTabs[index]);
				_editorTabs.Erase(index);
			}
			else
			{
				++index;
			}
		}

		// In the reverse order of their loading (a plugin's goes before the ones of the plugins it uses), while their code is still loaded
		_editorModuleHost.StopAll();
		for (auto it = _editorModuleLibraries.RBegin(); it != _editorModuleLibraries.REnd(); ++it)
		{
			DefaultAllocator::GetInstance().Delete(*it);
		}
		_editorModuleLibraries.Clear();
		return true;
	}

	/// @brief
	/// @param path
	/// @return
	bool Editor::OpenProject(const Path& path)
	{
		if (Project::GetInstance()->Open(path) == false)
		{
			return false;
		}
		AddProjectInRecentProject(path);

		// Before the game module is loaded, even to test it (CanLoadProjectModules): it links their libraries, which are next to the editor,
		// not next to it (DynamicLibrary loads with LOAD_WITH_ALTERED_SEARCH_PATH: the libraries loaded already are found, not the others)
		if (_runtimeModuleHost == nullptr && LoadRuntimeModules() == false)
		{
			return false;
		}

		// Missing or outdated (linked against engine libraries that changed since) game module, rebuild it
		if (Project::GetInstance()->HasGameModule() == false || Project::GetInstance()->CanLoadProjectModules() == false)
		{
			ImGuiManager::GetInstance()->CloseAllWindow();
			ImGuiManager::GetInstance()->OpenWindow<MissingProjectModuleModal>();
			return true;
		}
		else
		{
			return LoadEditor();
		}
	}

	bool Editor::LoadEditor()
	{
		// The editor modules of the plugins the project uses (their runtime modules are loaded, see OpenProject), before its own
		if (_modulesLoaded == false)
		{
			_modulesLoaded = true;
			if (LoadEditorModules() == false)
			{
				return false;
			}
		}

		if (Project::GetInstance()->ReloadProjectModules() == false)
		{
			return false;
		}

		AssetDatabase::GetInstance()->Init();
		VersionControl::GetInstance()->OnProjectOpened(*Project::GetInstance());

		ImGuiManager::GetInstance()->CloseAllWindow();

		ImGuiManager::GetInstance()->SetDrawCallback(
			[this]()
			{
				VersionControl::GetInstance()->Update(static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow())->IsFocused());

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
					ImGuiViewport* mainViewport = ImGui::GetMainViewport();
					ImGui::SetCursorScreenPos(mainViewport->Pos + ImVec2(64.0f, 0.0f));

					if (ImGui::BeginMenu("File") == true)
					{
						// The selected tab, as CTRL+S (see EditorTab::HandleSaveShortcut)
						EditorTab* selectedTab = GetSelectedEditorTab();
						bool       canSave = selectedTab != nullptr && selectedTab->IsDirty() && selectedTab->GetAsset() != nullptr;
						bool       canSaveAll = false;
						for (EditorTab* editorTab : _editorTabs)
						{
							canSaveAll |= editorTab->IsDirty();
						}
						if (ImGui::MenuItem("Save", "CTRL+S", false, canSave) == true)
						{
							selectedTab->Save();
						}
						if (ImGui::MenuItem("Save All", nullptr, false, canSaveAll) == true)
						{
							Editor::GetInstance()->Save();
						}

						if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S", false, selectedTab != nullptr && selectedTab->CanSaveAs()) == true)
						{
							selectedTab->RequestSaveAs();
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Build") == true)
						{
							Editor::GetInstance()->Build(BuildPlatform::Windows);
						}
						if (ImGui::MenuItem("Build Android") == true)
						{
							Editor::GetInstance()->Build(BuildPlatform::Android);
						}
						if (ImGui::MenuItem("Build and Run") == true)
						{
							Editor::GetInstance()->BuildAndRun();
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Quit") == true)
						{
							// Application::GetInstance()->Quit();
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Edit") == true)
					{
						// The selected tab's history (see EditorTab::Undo)
						EditorTab* selectedTab = GetSelectedEditorTab();
						bool       canUndo = selectedTab != nullptr && selectedTab->CanUndo();
						bool       canRedo = selectedTab != nullptr && selectedTab->CanRedo();
						// "###": the item keeps its id whatever the label
						String undoLabel = (canUndo && selectedTab->GetUndoLabel().Empty() == false) ? "Undo " + selectedTab->GetUndoLabel() : String("Undo");
						String redoLabel = (canRedo && selectedTab->GetRedoLabel().Empty() == false) ? "Redo " + selectedTab->GetRedoLabel() : String("Redo");
						undoLabel += "###Undo";
						redoLabel += "###Redo";
						if (ImGui::MenuItem(undoLabel.CStr(), "CTRL+Z", false, canUndo))
						{
							selectedTab->Undo();
						}
						if (ImGui::MenuItem(redoLabel.CStr(), "CTRL+Y", false, canRedo))
						{
							selectedTab->Redo();
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Project Settings") == true)
						{
							ProjectSettingsWindow::Open();
						}
						if (ImGui::MenuItem("Plugins") == true)
						{
							PluginsWindow::Open();
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Window") == true)
					{
						if (ImGui::MenuItem("Asset Browser") == true)
						{
							// A standalone one, beside the floating one of the bottom bar
							ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>()->CenterOnCurrentWindow();
						}
						if (ImGui::MenuItem("Version Control Changes") == true)
						{
							VersionControlChangesWindow::Open();
						}
						if (ImGui::MenuItem("ImGui demo"))
						{
							ImguiDemo = !ImguiDemo;
						}

						WindowFactory::GetInstance()->Draw();

						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Hot Reload") == true)
					{
						Project::GetInstance()->UnloadProjectModules(); // Before the engine's editor modules: loaded after them
						UnloadEditorModules();
						LoadEditorModules();
						Project::GetInstance()->ReloadProjectModules();
					}
					float menusEndX = ImGui::GetItemRectMax().x;

					ImVec2 titleBarPos = ImGui::GetWindowPos();
					ImVec2 titleBarSize = ImGui::GetWindowSize();
					float  controlsPosX = titleBarPos.x + titleBarSize.x - TitleBar::GetWindowControlsWidth();

					_fpsCounter.OnFrame();
					char fps[64] = {'\0'};
					fmt::format_to_n(fps, sizeof(fps) - 1, "{} fps", (uint32_t)_fpsCounter.GetFPS());
					String projectName = Project::GetInstance()->GetName();
					float  itemSpacing = ImGui::GetStyle().ItemSpacing.x;
					// Fixed slot for the fps so the project name doesn't move when the digit count changes
					float  fpsSlotWidth = ImGui::CalcTextSize("9999 fps").x;
					float  fpsSlotPosX = controlsPosX - titleBarPos.x - fpsSlotWidth - itemSpacing * 2.0f;
					ImGui::BeginDisabled();
					ImGui::SameLine(fpsSlotPosX - ImGui::CalcTextSize(projectName.CStr()).x - itemSpacing * 3.0f);
					ImGui::TextUnformatted(projectName.CStr());
					ImGui::SameLine(fpsSlotPosX + fpsSlotWidth - ImGui::CalcTextSize(fps).x);
					ImGui::TextUnformatted(fps);
					ImGui::EndDisabled();

					// Custom title bar: empty space of the bar drags the window, menus stay clickable
					float        menusStartX = mainViewport->Pos.x + 64.0f;
					Vector<Rect> interactiveAreas;
					interactiveAreas.push_back(Rect {Vector2(menusStartX, titleBarPos.y), Vector2(menusEndX - menusStartX, titleBarSize.y)});
					TitleBar::Draw(Rect {Vector2(titleBarPos.x, titleBarPos.y), Vector2(titleBarSize.x, titleBarSize.y)}, interactiveAreas);

					// In the menu bar's draw list: under the other windows and the popups, dimmed by a modal. It goes past the bottom of
					// the bar (own clip rect) over the empty left column of TabBarContainer, kept behind the bar (NoBringToFrontOnFocus).
					ImVec2      logoMin = mainViewport->Pos + ImVec2((64.0f - 48.0f) * 0.5f, 0.0f);
					ImVec2      logoMax = logoMin + ImVec2(48.0f, 48.0f);
					ImDrawList* menuBarDrawList = ImGui::GetWindowDrawList();
					menuBarDrawList->PushClipRect(logoMin, logoMax, false);
					menuBarDrawList->AddImage(_hodTexture, logoMin, logoMax);
					menuBarDrawList->PopClipRect();

					ImGui::EndMainMenuBar();
				}

				ImGuiViewport* mainViewport = ImGui::GetMainViewport();

				float statusBarHeight = 32;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
				ImGui::SetNextWindowPos(mainViewport->Pos + ImVec2(0, ImGui::GetFrameHeight()));
				ImGui::SetNextWindowSize(mainViewport->Size - ImVec2(0, ImGui::GetFrameHeight() + statusBarHeight));
				ImGui::SetNextWindowViewport(mainViewport->ID);
				bool open = ImGui::Begin("TabBarContainer", nullptr,
			                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
			                                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);
				if (open)
				{
					if (_editorTabs.Empty() == false)
					{
						ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(64.0f, 2.0f));
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
						bool visible =
							ImGui::BeginTabBar("EditorTabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_DrawSelectedOverline);
						ImGui::PopStyleVar();
						if (visible)
						{
							// Index based: a tab can open another one while drawing (OpenAsset push_back may reallocate)
							uint32_t index = 0;
							while (index < _editorTabs.Size())
							{
								EditorTab* editorTab = _editorTabs[index];
								if (editorTab->Draw() == false)
								{
									DefaultAllocator::GetInstance().Delete(editorTab);
									_editorTabs.Erase(index);
								}
								else
								{
									++index;
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

				ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
			                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::SetNextWindowPos(mainViewport->Pos + ImVec2(0, mainViewport->Size.y - statusBarHeight));
				ImGui::SetNextWindowSize(ImVec2(mainViewport->Size.x, statusBarHeight));
				ImGui::SetNextWindowViewport(mainViewport->ID);
				bool draw = ImGui::Begin("Bottom Menu Bar", nullptr, window_flags);
				ImGui::PopStyleVar(2);
				if (draw)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
					ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 12.0f);
					ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_Space, ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_Tooltip);
					if (ImGui::Button(ICON_MDI_FOLDER_SEARCH " Asset Browser", ImVec2(0.0f, statusBarHeight)))
					{
						// Without editor tab, it is all there is to show: stays open (refocused)
						_showFloatingAssetBrowser = _showFloatingAssetBrowser == false || _editorTabs.Empty();
						if (_showFloatingAssetBrowser)
						{
							_focusFloatingAssetBrowserWindow = true;
						}
					}
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
					ImGui::SameLine();
					if (ImGui::Button(ICON_MDI_TEXT_BOX " Output Log", ImVec2(0.0f, statusBarHeight)))
					{
					}
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
					ImGui::SameLine(ImGui::GetIO().DisplaySize.x - CalculateButtonSize(ICON_MDI_CIRCLE_OFF_OUTLINE " Tasks").x - 2);
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
					ImGui::SameLine();
					if (ImGui::Button(ICON_MDI_CIRCLE_OFF_OUTLINE " Tasks", ImVec2(0.0f, statusBarHeight)))
					{
						_showFloatingTaskTracker = !_showFloatingTaskTracker;
						if (_showFloatingTaskTracker)
						{
							_focusFloatingTaskTrackerWindow = true;
						}
					}
					ImGui::PopStyleVar(3);
					ImGui::PopStyleColor();
				}
				ImGui::End();

				// The tab bar above reopens it only on the next frame: without editor tab (the last one just closed), it never starts sliding down
				if (_showFloatingAssetBrowser || _editorTabs.Empty())
				{
					_floatingAssetBrowserWindowPos = std::min(350.0f, _floatingAssetBrowserWindowPos + ImGui::GetIO().DeltaTime * 2000.0f);
				}
				else
				{
					_floatingAssetBrowserWindowPos = std::max(0.0f, _floatingAssetBrowserWindowPos - ImGui::GetIO().DeltaTime * 2000.0f);
				}
				if (_floatingAssetBrowserWindowPos > 0.0f)
				{
					ImGui::SetNextWindowPos(mainViewport->Pos + ImVec2(20.0f, mainViewport->Size.y - statusBarHeight - _floatingAssetBrowserWindowPos));
					ImGui::SetNextWindowSize(ImVec2(mainViewport->Size.x - 40.0f, 350));
					ImGui::SetNextWindowViewport(mainViewport->ID);
					if (_focusFloatingAssetBrowserWindow)
					{
						_focusFloatingAssetBrowserWindow = false;
						ImGui::SetNextWindowFocus();
					}
					_floatingAssetBrowserWindow->Draw();
				}

				if (_showFloatingTaskTracker)
				{
					_floatingTaskTrackerWindowPos = std::min(555.0f, _floatingTaskTrackerWindowPos + ImGui::GetIO().DeltaTime * 4000.0f);
				}
				else
				{
					_floatingTaskTrackerWindowPos = std::max(0.0f, _floatingTaskTrackerWindowPos - ImGui::GetIO().DeltaTime * 4000.0f);
				}
				if (_floatingTaskTrackerWindowPos > 0.0f)
				{
					ImGui::SetNextWindowPos(mainViewport->Pos + ImVec2(mainViewport->Size.x - 350.0f - 20.0f, mainViewport->Size.y - statusBarHeight - _floatingTaskTrackerWindowPos));
					ImGui::SetNextWindowSize(ImVec2(350.0f, 555.0f));
					ImGui::SetNextWindowViewport(mainViewport->ID);
					if (_focusFloatingTaskTrackerWindow)
					{
						_focusFloatingTaskTrackerWindow = false;
						ImGui::SetNextWindowFocus();
					}
					_floatingTaskTrackerWindow->Draw();
				}
			});

		// OpenEditorTab<SceneEditorTab>();
		_showFloatingAssetBrowser = true;
		_focusFloatingAssetBrowserWindow = true;

		DesktopWindow* mainWindow = static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow());
		mainWindow->Maximize();
		return true;
	}

	bool Editor::AddProjectInRecentProject(const Path& path) const
	{
		RecentProjects recentProjects;

		Path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("Project.json");

		// Read and deserialize existing file
		{
			Document           document;
			DocumentReaderJson jsonReader;
			if (jsonReader.Read(document, projectsPath) == true)
			{
				Serializer::Deserialize(recentProjects, document.GetRootNode());
			}
		}

		bool alreadyExist = false;
		for (const String& projectPath : recentProjects._projectsPath)
		{
			if (projectPath == path.GetString())
			{
				alreadyExist = true;
				break;
			}
		}

		if (alreadyExist == false)
		{
			recentProjects._projectsPath.push_back(path.GetString().CStr());

			Document writeDocument;
			Serializer::Serialize(recentProjects, writeDocument.GetRootNode());

			FileSystem::GetInstance()->CreateDirectories(projectsPath.ParentPath());
			DocumentWriterJson jsonWriter;
			jsonWriter.Write(writeDocument, projectsPath);
		}

		return true;
	}

	/// @brief
	/// @param path
	/// @return
	bool Editor::CreateProject(const Project::CreateInfo& createInfo, std::string_view versionControl)
	{
		if (Project::GetInstance()->Create(createInfo) == false)
		{
			return false;
		}
		if (versionControl.empty() == false)
		{
			VersionControl::RequestWorkspaceCreation(createInfo._directory, versionControl); // Its failure is reported: the project is there all the same
		}
		return OpenProject(Project::GetInstance()->GetProjectPath());
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

		World* world = World::GetInstance();
		if (world->SaveToDocument(worldDocument.GetRootNode()) == false)
		{
		    return false;
		}

		Scene scene;
		if (scene.DeserializeFromDocument(worldDocument.GetRootNode()) == false)
		{
		    return false;
		}

		_currentScene->Save(&scene, scene.GetReflectionDescriptorV());
	}
		*/

		bool result = true;
		for (EditorTab* editorTab : _editorTabs)
		{
			if (editorTab->IsDirty() == true && editorTab->Save() == false)
			{
				result = false;
			}
		}

		return result;
	}

	/// @brief
	/// @param assetType
	void Editor::CloseEditorTabsOf(uint64_t assetType)
	{
		uint32_t index = 0;
		while (index < _editorTabs.Size())
		{
			if (_editorTabs[index]->GetAsset() != nullptr && _editorTabs[index]->GetAsset()->GetType() == assetType)
			{
				DefaultAllocator::GetInstance().Delete(_editorTabs[index]);
				_editorTabs.Erase(index);
			}
			else
			{
				++index;
			}
		}
	}

	/// @brief
	/// @return
	EditorTab* Editor::GetSelectedEditorTab() const
	{
		for (EditorTab* editorTab : _editorTabs)
		{
			if (editorTab->IsSelected())
			{
				return editorTab;
			}
		}
		return nullptr;
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
				editorTab->Focus();
				_showFloatingAssetBrowser = false;
				return editorTab;
			}
		}

		const AssetType* assetType = AssetDatabase::GetInstance()->FindAssetType(asset->GetType());
		if (assetType == nullptr)
		{
			OUTPUT_WARNING("{}: its asset type is unknown, its plugin is disabled or missing (see Edit > Plugins)", asset->GetPath());
			return nullptr;
		}
		EditorTab* editorTab = assetType->CreateEditorTab(asset);
		if (editorTab == nullptr)
		{
			return nullptr; // todo message
		}

		_editorTabs.push_back(editorTab);

		_showFloatingAssetBrowser = false;

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

	    World* world = World::GetInstance();

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

	    World* world = World::GetInstance();
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

	    World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief
	void Editor::Resume()
	{
	    if (_paused == false)
	    {
	        return;
	    }

	    _paused = false;

	    World::GetInstance()->SetEditorPaused(_paused);
	}

	/// @brief
	void Editor::PlayNextFrame()
	{
	    Pause();
	    World::GetInstance()->EditorNextFrame();
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
	void Editor::Build(BuildPlatform buildPlatform)
	{
		Path buildDir = Project::GetInstance()->GetBuildDirPath();
		FileSystem::GetInstance()->CreateDirectories(buildDir);

		Path projectDir = Project::GetInstance()->GetProjectPath().ParentPath();
		Path intermediateDir = Project::GetInstance()->GetCacheBuildDirPath() / Path(CMakeHelper::GetCurrentPlatform());
		FileSystem::GetInstance()->CreateDirectories(intermediateDir);

		if (Project::GetInstance()->GenerateGameModuleCMakeList() == false)
		{
			return;
		}

		if (buildPlatform != BuildPlatform::Android)
		{
			if (CMakeHelper::Configure(projectDir, intermediateDir, buildDir, CMakeHelper::Target::Retail) == false)
			{
				return;
			}

			if (CMakeHelper::Build(intermediateDir, "Release") == false)
			{
				return;
			}

			if (CMakeHelper::Install(intermediateDir, "Release") == false)
			{
				return;
			}
		}

		// bootInfo._startupScene = Project::GetInstance()->GetStartupScene();
		// bootInfo._gameModule   = Project::GetInstance()->GetName();

		Path dataDirPath = buildDir / "Datas";
		FileSystem::GetInstance()->CreateDirectories(dataDirPath);

		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(Project::GetInstance()->GetResourceDirPath().GetString().CStr()))
			{
				const std::filesystem::path& source = entry.path();
				if (std::filesystem::is_regular_file(source))
				{
					std::filesystem::path destination = dataDirPath.GetString().CStr() / source.filename();
					std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
				}
			}

			// Loaded by Application::BootGame, absent when every setting has its default value
			Path projectSettingsDirPath = Project::GetInstance()->GetProjectSettingsDirPath();
			if (FileSystem::GetInstance()->Exists(projectSettingsDirPath))
			{
				Path dataProjectSettingsDirPath = dataDirPath / "ProjectSettings";
				std::filesystem::remove_all(dataProjectSettingsDirPath.GetString().CStr()); // Without the settings files removed since
				std::filesystem::copy(projectSettingsDirPath.GetString().CStr(), dataProjectSettingsDirPath.GetString().CStr(), std::filesystem::copy_options::recursive);
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_ERROR("Build error : {}", e.what());
			return;
		}

		if (buildPlatform == BuildPlatform::Android)
		{
			Path source = FileSystem::GetExecutablePath().ParentPath() / "Templates" / "AndroidStudio";
			Path destination = buildDir / "AndroidStudioProject";

			try
			{
				const auto copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;

				std::filesystem::copy(source.GetString().CStr(), destination.GetString().CStr(), copyOptions);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				OUTPUT_ERROR("Build error : {}", e.what());
			}

			Path cmakeLists = projectDir / "CMakeLists.txt";

			Path   installDir(HOD_INSTALL_PREFIX);
			Path   externalJarDirPath = installDir / "platforms" / "android" / "jar";
			Path   jniLibsDirPath = installDir / "platforms" / "android" / "jniLibs";
			Path   engineDirPath = installDir / "cmake";
			String gameBuildType = "Application";

			engineDirPath.PortableSeparator();

			std::string cmakeArgs = fmt::format("\"-DHodEngine_DIR={}\", \"-DHOD_GAME_BUILD_TYPE={}\"", engineDirPath, gameBuildType);

			{
				Path   buildGradlePath = buildDir / "AndroidStudioProject" / "app" / "build.gradle.kts";
				String content;
				if (FileSystem::GetInstance()->ReadAllText(buildGradlePath, content) == false)
				{
					OUTPUT_ERROR("Build error : cannot open {}", buildGradlePath.GetString().CStr());
					return;
				}

				auto replaceAll = [](String& str, const char* from, const char* to)
				{
					uint32_t fromLen = (uint32_t)strlen(from);
					uint32_t pos = 0;
					while ((pos = str.Find(from, pos)) != String::Npos)
					{
						str.Replace(pos, fromLen, to);
						pos += (uint32_t)strlen(to);
					}
				};

				dataDirPath.PortableSeparator();
				cmakeLists.PortableSeparator();
				externalJarDirPath.PortableSeparator();
				jniLibsDirPath.PortableSeparator();
				replaceAll(content, "[[PROJECT_GAME_DATAS]]", dataDirPath.GetString().CStr());
				replaceAll(content, "[[PROJECT_GAME_CMAKELIST]]", cmakeLists.GetString().CStr());
				replaceAll(content, "[[PROJECT_CMAKE_ARGS]]", cmakeArgs.c_str());
				replaceAll(content, "[[EXTERNAL_JAR_DIR]]", externalJarDirPath.GetString().CStr());
				replaceAll(content, "[[JNI_LIBS_DIR]]", jniLibsDirPath.GetString().CStr());

				if (FileSystem::GetInstance()->WriteAllText(buildGradlePath, content) == false)
				{
					OUTPUT_ERROR("Build error : cannot write {}", buildGradlePath.GetString().CStr());
					return;
				}
			}

			{
				Path   manifestPath = buildDir / "AndroidStudioProject" / "app" / "src" / "main" / "AndroidManifest.xml";
				String content;
				if (FileSystem::GetInstance()->ReadAllText(manifestPath, content) == false)
				{
					OUTPUT_ERROR("Build error : cannot open {}", manifestPath.GetString().CStr());
					return;
				}

				String   projectGameName = Project::GetInstance()->GetName() + "Game";
				uint32_t pos = 0;
				while ((pos = content.Find("[[PROJECT_GAME_NAME]]", pos)) != String::Npos)
				{
					content.Replace(pos, 21, projectGameName);
					pos += projectGameName.Length();
				}

				if (FileSystem::GetInstance()->WriteAllText(manifestPath, content) == false)
				{
					OUTPUT_ERROR("Build error : cannot write {}", manifestPath.GetString().CStr());
					return;
				}
			}
		}
	}

	/// @brief
	void Editor::BuildAndRun()
	{
		Build(BuildPlatform::Windows);
		// todo run
	}

	void Editor::OpenImportDialog(const Path& path)
	{
		pfd::settings::verbose(true);
		pfd::open_file dialog("Import", Project::GetInstance()->GetSourceDirPath().GetString().CStr(), { "All Files", "*" }, pfd::opt::multiselect);
		auto           result = dialog.result();

		if (result.size() == 1)
		{
			Editor::Import(result[0].c_str(), path);
		}
		else if (result.size() > 1)
		{
			Vector<Path> sourceFilePaths;
			sourceFilePaths.Reserve((uint32_t)dialog.result().size());
			for (const auto& result : dialog.result())
			{
				sourceFilePaths.PushBack(result.c_str());
			}
			Editor::Import(sourceFilePaths, path);
		}
	}

	void Editor::Import(const Path& sourceFilePath, const Path& destinationDirPath)
	{
		ImportJob* importJob = DefaultAllocator::GetInstance().New<ImportJob>(sourceFilePath, destinationDirPath);
		importJob->SetAutoDelete(true);
		JobScheduler::GetInstance()->PushBackground(importJob);
	}

	void Editor::Import(const Vector<Path>& sourceFilePaths, const Path& destinationDirPath)
	{
		for (const Path& sourceFilePath : sourceFilePaths)
		{
			Editor::Import(sourceFilePath, destinationDirPath);
		}
	}

	void Editor::Cook(const Path& asset)
	{
		CookJob* cookJob =
			DefaultAllocator::GetInstance().New<CookJob>(asset, std::to_underlying(GetEditorPlatform()), std::to_underlying(Config::Debug), std::to_underlying(ResourceVariant::Language::All));
		cookJob->SetAutoDelete(true);
		JobScheduler::GetInstance()->PushBackground(cookJob);
	}
}
