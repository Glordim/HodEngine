#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorModule.hpp"
#include <HodEngine/Core/ModuleHost.hpp>
#include <HodEngine/Core/RuntimeModule.hpp>
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"
#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"


#include "HodEngine/GameSystems/Resource/ResourceVariant.hpp"
#include <algorithm>
#include <filesystem>
#include <fmt/format.h>

#include "FileTemplates/CMakeListsEditor.txt.hpp"
#include "FileTemplates/CMakeListsGame.txt.hpp"
#include "FileTemplates/CMakeListsRoot.txt.hpp"
#include "FileTemplates/Component.cpp.hpp"
#include "FileTemplates/Component.hpp.hpp"
#include "FileTemplates/EditorModule.cpp.hpp"
#include "FileTemplates/Export.hpp.hpp"
#include "FileTemplates/Module.cpp.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Editor/CMakeHelper.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include <memory>
#include <utility>


// todo move ?
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

//

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(Project, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Project::_name, "_name");
		AddPropertyT(reflectionDescriptor, &Project::_startupScene, "_startupScene");
		AddPropertyT(reflectionDescriptor, &Project::_disabledModules, "_disabledModules");
	}

	_SingletonConstructor(Project) {}

	Project::~Project()
	{
		UnloadProjectModules();
	}

	/// @brief
	/// @param directory
	/// @return
	bool Project::Create(const CreateInfo& createInfo)
	{
		const Path& directory = createInfo._directory;
		if (String error = CheckNewProjectDirectory(directory); error.Empty() == false)
		{
			OUTPUT_ERROR("Project::Create fail: {}", error);
			return false;
		}

		if (FileSystem::GetInstance()->Exists(directory) == false && FileSystem::GetInstance()->CreateDirectories(directory) == false)
		{
			OUTPUT_ERROR("Project::Create fail: can't create the folder {}", directory);
			return false;
		}

		_projectPath = directory / (directory.Filename().GetString() + ".hod");
		_name = _projectPath.Stem().GetString();
		_disabledModules = createInfo._disabledModules;

		if (CreateMinimalSourceForModule(directory) == false)
		{
			return false;
		}

		return Save();
	}

	/// @brief The project name is used for the CMake targets and the C++ export macros of its modules, it must be a valid C identifier
	/// @param name
	/// @return
	bool Project::IsValidName(const String& name)
	{
		if (name.Empty() || (name[0] >= '0' && name[0] <= '9'))
		{
			return false;
		}
		for (char c : name)
		{
			bool valid = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
			if (valid == false)
			{
				return false;
			}
		}
		return true;
	}

	/// @brief
	/// @param directory
	/// @return
	String Project::CheckNewProjectDirectory(const Path& directory)
	{
		if (directory.Empty())
		{
			return "Choose where to create the project";
		}
		if (directory.IsAbsolute() == false)
		{
			return String::Format("%s isn't an absolute path", directory.GetString().CStr());
		}
		const String name = directory.Filename().GetString();
		if (name.Empty())
		{
			return "Name the project";
		}
		if (IsValidName(name) == false)
		{
			return String::Format("'%s' isn't a valid project name: letters, digits and '_' only, not starting with a digit", name.CStr());
		}
		if (FileSystem::GetInstance()->Exists(directory) == false)
		{
			return String();
		}
		if (FileSystem::GetInstance()->IsDirectory(directory) == false)
		{
			return String::Format("%s is a file", directory.GetString().CStr());
		}
		std::error_code errorCode;
		bool            empty = std::filesystem::is_empty(directory.GetString().CStr(), errorCode);
		if (errorCode)
		{
			return String::Format("%s can't be read: %s", directory.GetString().CStr(), errorCode.message().c_str());
		}
		if (empty == false)
		{
			return String::Format("The folder %s already exists and isn't empty", directory.GetString().CStr());
		}
		return String();
	}

	/// @brief
	/// @param moduleGraph
	/// @return
	bool Project::BuildEngineModuleGraph(ModuleGraph& moduleGraph)
	{
		Vector<ModuleManifest> manifests;
		bool                   result = ModuleGraph::LoadManifests(FileSystem::GetExecutablePath().ParentPath() / "Modules", manifests);
		result &= moduleGraph.Build(manifests);
		return result;
	}

	/// @brief Write a module source file from a template, replacing [[MODULE_NAME]], [[MODULE_API]] and [[MODULE_EXPORT]]
	/// @param path
	/// @param templateContent
	/// @param moduleName ex: CircleGame
	/// @param macroPrefix ex: Circle_GAME (uppercased to give CIRCLE_GAME_API / CIRCLE_GAME_EXPORT)
	/// @return
	bool Project::WriteModuleSourceFile(const Path& path, std::string_view templateContent, const String& moduleName, const String& macroPrefix)
	{
		String moduleApi = macroPrefix + "_API";
		String moduleExport = macroPrefix + "_EXPORT";
		std::transform(moduleApi.begin(), moduleApi.end(), moduleApi.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });
		std::transform(moduleExport.begin(), moduleExport.end(), moduleExport.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });

		String content(templateContent);
		const std::pair<std::string_view, const String&> replacements[] = {
			{"[[MODULE_NAME]]", moduleName},
			{"[[MODULE_API]]", moduleApi},
			{"[[MODULE_EXPORT]]", moduleExport},
		};
		for (const auto& [tag, value] : replacements)
		{
			uint32_t replaceIndex = content.Find(tag);
			while (replaceIndex != String::Npos)
			{
				content.Replace(replaceIndex, static_cast<uint32_t>(tag.size()), value);
				replaceIndex = content.Find(tag, replaceIndex + value.Size());
			}
		}

		if (FileSystem::GetInstance()->WriteAllText(path, content) == false)
		{
			OUTPUT_ERROR("Failed to generate MinimalSourceForModule at {}", path);
			return false;
		}
		OUTPUT_MESSAGE("MinimalSourceForModule generated at {}", path);
		return true;
	}

	/// @brief Generate the minimal sources of the game and editor modules
	/// Layout expected by GenerateGameModuleCMakeList: Code/<Name>Game and Code/<Name>Editor
	/// @param directory
	/// @return
	bool Project::CreateMinimalSourceForModule(const Path& directory)
	{
		// Game module (macro prefix gives <NAME>_GAME_EXPORT, matching GenerateGameModuleCMakeList)
		String gameModuleName = _name + "Game";
		String gameMacroPrefix = _name + "_GAME";
		Path   gameDirPath = directory / "Code" / gameModuleName;
		Path   gameComponentsDirPath = gameDirPath / "Components";
		if (FileSystem::GetInstance()->Exists(gameComponentsDirPath) == false && FileSystem::GetInstance()->CreateDirectories(gameComponentsDirPath) == false)
		{
			return false;
		}

		// Editor module
		String editorModuleName = _name + "Editor";
		String editorMacroPrefix = _name + "_EDITOR";
		Path   editorDirPath = directory / "Code" / editorModuleName;
		if (FileSystem::GetInstance()->Exists(editorDirPath) == false && FileSystem::GetInstance()->CreateDirectories(editorDirPath) == false)
		{
			return false;
		}

		struct FileToWrite
		{
			Path             _path;
			std::string_view _template;
			const String&    _moduleName;
			const String&    _macroPrefix;
		};

		const FileToWrite files[] = {
			{gameDirPath / "Export.hpp", Export_hpp, gameModuleName, gameMacroPrefix},
			{gameDirPath / (gameModuleName + "Module.cpp").CStr(), Module_cpp, gameModuleName, gameMacroPrefix},
			{gameComponentsDirPath / "MyFirstComponent.hpp", Component_hpp, gameModuleName, gameMacroPrefix},
			{gameComponentsDirPath / "MyFirstComponent.cpp", Component_cpp, gameModuleName, gameMacroPrefix},
			{editorDirPath / "Export.hpp", Export_hpp, editorModuleName, editorMacroPrefix},
			{editorDirPath / (editorModuleName + "Module.cpp").CStr(), EditorModule_cpp, editorModuleName, editorMacroPrefix},
		};
		for (const FileToWrite& file : files)
		{
			if (FileSystem::GetInstance()->Exists(file._path))
			{
				continue; // Never overwrite user code
			}
			if (WriteModuleSourceFile(file._path, file._template, file._moduleName, file._macroPrefix) == false)
			{
				return false;
			}
		}

		return true;
	}

	/// @brief
	/// @param projectPath
	/// @return
	bool Project::Open(const Path& projectPath)
	{
		if (FileSystem::GetInstance()->Exists(projectPath) == false)
		{
			OUTPUT_ERROR("Project::Open fail: {} doesn't exist", projectPath);
			return false;
		}

		if (FileSystem::SetWorkingDirectory(projectPath.ParentPath()) == false)
		{
			return false;
		}

		_projectPath = projectPath;
		_name = _projectPath.Stem().GetString();

		_sourceDirPath = _projectPath.ParentPath() / "Source";
		if (FileSystem::GetInstance()->Exists(_sourceDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_sourceDirPath) == false)
		{
			return false;
		}

		_assetDirPath = _projectPath.ParentPath() / "Asset";
		if (FileSystem::GetInstance()->Exists(_assetDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_assetDirPath) == false)
		{
			return false;
		}

		_resourceDirPath = _projectPath.ParentPath() / "Cache" / "Resources";
		if (FileSystem::GetInstance()->Exists(_resourceDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_resourceDirPath) == false)
		{
			return false;
		}

		_cacheBuildDirPath = _projectPath.ParentPath() / "Cache" / "Build";
		if (FileSystem::GetInstance()->Exists(_cacheBuildDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_cacheBuildDirPath) == false)
		{
			return false;
		}

		_buildDirPath = _projectPath.ParentPath() / "Build";
		if (FileSystem::GetInstance()->Exists(_buildDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_buildDirPath) == false)
		{
			return false;
		}

		_tmpDirPath = _projectPath.ParentPath() / "Cache" / "Tmp";
		if (FileSystem::GetInstance()->Exists(_tmpDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_tmpDirPath) == false)
		{
			return false;
		}

		_projectSettingsDirPath = _projectPath.ParentPath() / "ProjectSettings";

		_codeDirPath = _projectPath.ParentPath() / "Code";

		ResourceManager::GetInstance()->SetResourceDirectory(_resourceDirPath / CMakeHelper::GetCurrentPlatform() / ToString(Config::Debug)); // TODO uniformize Config and Platform setup
		ResourceManager::GetInstance()->SetFileNotFoundCallback(+[](const UID& uid) {
			std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->Find(uid);
			if (asset == nullptr)
			{
				return false;
			}
			uint64_t type = asset->GetType();
			Cooker* cooker = AssetDatabase::GetInstance()->AcquireCooker(type);
			if (cooker == nullptr)
			{
				return false;
			}
			bool result = cooker->Cook(*asset, std::to_underlying(GetEditorPlatform()), std::to_underlying(Config::Debug), std::to_underlying(ResourceVariant::Language::All), 0);
			AssetDatabase::GetInstance()->ReleaseCooker(cooker);
			return result;
		});
		ResourceManager::GetInstance()->SetCheckUpToDateCallback(+[](const UID& uid, const ResourceContainer& resourceContainer) {
			std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->Find(uid);
			if (asset == nullptr)
			{
				return false;
			}
			uint64_t type = asset->GetType();
			Cooker* cooker = AssetDatabase::GetInstance()->AcquireCooker(type);
			if (cooker == nullptr)
			{
				return false;
			}
			if (asset->GetContentHash() == resourceContainer.GetAssetContentHash() && cooker->GetCookerVersion() == resourceContainer.GetCookerVersion())
			{
				AssetDatabase::GetInstance()->ReleaseCooker(cooker);
				return true;
			}
			bool result = cooker->Cook(*asset, std::to_underlying(GetEditorPlatform()), std::to_underlying(Config::Debug), std::to_underlying(ResourceVariant::Language::All), 0);
			AssetDatabase::GetInstance()->ReleaseCooker(cooker);
			return result;
		});

		String gameModuleName = _name + "Game";
		Path   moduleBuildDirectoryPath = GetCacheBuildDirPath() / "Editor";
		_gameModule.Init(moduleBuildDirectoryPath / "Output" / "platforms" / Path(CMakeHelper::GetCurrentPlatform()) / Path(CMakeHelper::GetCurrentAbi()) / "shared" / "bin" /
		                     gameModuleName,
		                 true);
		FileSystem::GetInstance()->CreateDirectories(_gameModule.GetPath().ParentPath());
		if (_gameModuleFileSystemWatcher.Init(_gameModule.GetPath(), nullptr, nullptr, [this](const Path&) { ReloadProjectModules(); }, nullptr) == false)
		{
			return false;
		}

		String editorModuleName = _name + "Editor";
		_editorModule.Init(moduleBuildDirectoryPath / "Output" / "platforms" / Path(CMakeHelper::GetCurrentPlatform()) / Path(CMakeHelper::GetCurrentAbi()) / "shared" / "bin" /
		                       editorModuleName,
		                   false);
		FileSystem::GetInstance()->CreateDirectories(_editorModule.GetPath().ParentPath());
		if (_editorModuleFileSystemWatcher.Init(_editorModule.GetPath(), nullptr, nullptr, [this](const Path&) { ReloadProjectModules(); }, nullptr) == false)
		{
			return false;
		}

		if (Load() == false)
		{
			return false;
		}
		ResolveModules();
		return true;
	}

	/// @brief The engine modules described next to the editor (see ModuleManifest), without the ones the project disables
	void Project::ResolveModules()
	{
		BuildEngineModuleGraph(_moduleGraph);
		_moduleGraph.Resolve(_disabledModules);

		for (const String& error : _moduleGraph.GetErrors())
		{
			OUTPUT_ERROR("Modules: {}", error);
		}
		for (const ModuleGraph::Module& module : _moduleGraph.GetModules())
		{
			if (module._enabled == false)
			{
				OUTPUT_MESSAGE("Module '{}' disabled: {}", module._manifest._name, module._disabledReason);
			}
		}
	}

	/// @brief
	/// @return
	const ModuleGraph& Project::GetModuleGraph() const
	{
		return _moduleGraph;
	}

	/// @brief
	/// @return
	const Vector<String>& Project::GetDisabledModules() const
	{
		return _disabledModules;
	}

	/// @brief
	/// @param disabledModules
	/// @return
	bool Project::SetDisabledModules(const Vector<String>& disabledModules)
	{
		String error;
		if (VersionControl::GetInstance() != nullptr && VersionControl::GetInstance()->PrepareWrite(ArrayView<Path>(&_projectPath, 1), &error) == false)
		{
			OUTPUT_ERROR("Can't save {}: {}", _projectPath, error);
			return false;
		}
		Vector<String> previous = _disabledModules;
		_disabledModules = disabledModules;
		if (Save() == false)
		{
			_disabledModules = previous;
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	Path Project::GetGameModuleRuntimeLibrariesStampPath() const
	{
		return GetCacheBuildDirPath() / "Editor" / "HodModules.txt";
	}

	/// @brief
	/// @return
	String Project::GetRuntimeLibrariesStamp() const
	{
		String stamp;
		for (const String& runtimeLibrary : _moduleGraph.GetEnabledRuntimeLibraries())
		{
			stamp += runtimeLibrary;
			stamp += "\n";
		}
		return stamp;
	}

	/// @brief
	/// @return
	bool Project::Load()
	{
		Document           document;
		DocumentReaderJson reader;
		reader.Read(document, _projectPath);

		Serializer::Deserialize(*this, document.GetRootNode());

		ProjectSettingsManager::GetInstance()->Load(_projectSettingsDirPath);

		return true;
	}

	/// @brief
	/// @return
	bool Project::Save()
	{
		Document document;
		Serializer::Serialize(*this, document.GetRootNode());

		DocumentWriterJson writer;
		writer.Write(document, _projectPath);

		return true;
	}

	/// @brief
	/// @param asset
	void Project::SetStartupScene(std::shared_ptr<Asset> asset)
	{
		_startupScene = asset->GetUid();
		Save();
	}

	/// @brief
	/// @return
	const UID& Project::GetStartupScene() const
	{
		return _startupScene;
	}

	/// @brief
	/// @return
	const String Project::GetName() const
	{
		return _name;
	}

	/// @brief
	/// @return
	const Path& Project::GetProjectPath() const
	{
		return _projectPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetSourceDirPath() const
	{
		return _sourceDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetAssetDirPath() const
	{
		return _assetDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetResourceDirPath() const
	{
		return _resourceDirPath;
	}

	const Path& Project::GetCacheBuildDirPath() const
	{
		return _cacheBuildDirPath;
	}

	/// @brief Temporary files that must end up in the project (same volume as the Asset directory: moving them there is an atomic rename)
	/// @return
	const Path& Project::GetTmpDirPath() const
	{
		return _tmpDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetProjectSettingsDirPath() const
	{
		return _projectSettingsDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetCodeDirPath() const
	{
		return _codeDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetBuildDirPath() const
	{
		return _buildDirPath;
	}

	/// @brief
	/// @return
	bool Project::HasGameModule() const
	{
		return FileSystem::GetInstance()->Exists(_gameModule.GetPath());
	}

	/// @brief Try to load then unload the project modules, a module linked against outdated engine libraries fails to load
	/// @return
	bool Project::CanLoadProjectModules()
	{
		// Built with other modules than the ones the project uses now: it links the libraries of the modules it was built with
		String builtStamp;
		if (FileSystem::GetInstance()->Exists(GetGameModuleRuntimeLibrariesStampPath()) == false ||
		    FileSystem::GetInstance()->ReadAllText(GetGameModuleRuntimeLibrariesStampPath(), builtStamp) == false || builtStamp != GetRuntimeLibrariesStamp())
		{
			OUTPUT_MESSAGE("The game module was built with other engine modules, it must be rebuilt");
			return false;
		}

		bool gameModuleLoaded = _gameModule.Load();
		_gameModule.Unload();
		if (gameModuleLoaded == false)
		{
			return false;
		}

		if (FileSystem::GetInstance()->Exists(_editorModule.GetPath()))
		{
			bool editorModuleLoaded = _editorModule.Load();
			_editorModule.Unload();
			if (editorModuleLoaded == false)
			{
				return false;
			}
		}
		return true;
	}

	/// @brief
	/// @return
	Path Project::GetGameModulePath() const
	{
		return _gameModule.GetPath();
	}

	/// @brief
	/// @return
	bool Project::GenerateGameModuleCMakeList() const
	{
		String projectName = _name;

		String projectGameName = _name + "Game";
		String projectGameExport = _name + "_GAME_EXPORT";
		std::transform(projectGameExport.begin(), projectGameExport.end(), projectGameExport.begin(), [](char c) { return std::toupper(c); });

		// The runtime libraries of the enabled modules, dependencies first: linked by the game, started by a static game in that order
		// (the game's CMakeLists turns the list into links and the source calling them: hod_link_engine_modules... in HodEngineConfig.cmake), loaded by the editor from their libraries
		String moduleList;
		for (const String& runtimeLibrary : _moduleGraph.GetEnabledRuntimeLibraries())
		{
			moduleList += runtimeLibrary;
			moduleList += "\n";
		}

		bool   result = true;
		Path   moduleListPath = _projectPath.ParentPath() / "Cache" / "Generated" / "HodModules.txt";
		String existingModuleList;
		if (FileSystem::GetInstance()->Exists(moduleListPath) == false || FileSystem::GetInstance()->ReadAllText(moduleListPath, existingModuleList) == false ||
		    existingModuleList != moduleList) // Not configured again for nothing
		{
			result &= FileSystem::GetInstance()->CreateDirectories(moduleListPath.ParentPath()) && FileSystem::GetInstance()->WriteAllText(moduleListPath, moduleList);
		}

		result &= CMakeHelper::GenerateCMakeLists(_codeDirPath / projectGameName / "CMakeLists.txt", CMakeListsGame_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectName},
													  {"[[PROJECT_GAME_NAME]]", projectGameName},
													  {"[[PROJECT_GAME_EXPORT]]", projectGameExport},
												  });

		String projectEditorName = _name + "Editor";
		String projectEditorExport = _name + "_EDITOR_EXPORT";
		std::transform(projectEditorExport.begin(), projectEditorExport.end(), projectEditorExport.begin(), [](char c) { return std::toupper(c); });

		result &= CMakeHelper::GenerateCMakeLists(_codeDirPath / projectEditorName / "CMakeLists.txt", CMakeListsEditor_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectName},
													  {"[[PROJECT_GAME_NAME]]", projectGameName},
													  {"[[PROJECT_EDITOR_NAME]]", projectEditorName},
													  {"[[PROJECT_EDITOR_EXPORT]]", projectEditorExport},
												  });

		result &= CMakeHelper::GenerateCMakeLists(_projectPath.ParentPath() / "CMakeLists.txt", CMakeListsRoot_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectName},
													  {"[[PROJECT_GAME_NAME]]", projectGameName},
													  {"[[PROJECT_EDITOR_NAME]]", projectEditorName},
												  });

		return result;
	}

	/// @brief
	/// @param outputs
	/// @return
	bool Project::ConfigureGameModule() const
	{
		Path gameModuleSourceDirectoryPath = _projectPath.ParentPath();
		Path gameModuleBuildDirectoryPath = GetCacheBuildDirPath() / "Editor";

		try
		{
			if (FileSystem::GetInstance()->Exists(gameModuleBuildDirectoryPath) == false)
			{
				FileSystem::GetInstance()->CreateDirectories(gameModuleBuildDirectoryPath);
				OUTPUT_MESSAGE("Create Build directory at {}", gameModuleBuildDirectoryPath);
			}
			else
			{
				OUTPUT_MESSAGE("Build directory already exist at {}", gameModuleBuildDirectoryPath);
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_MESSAGE("Failed to create Build directory at {} : ", gameModuleBuildDirectoryPath, e.what());
			return false;
		}

		return CMakeHelper::Configure(gameModuleSourceDirectoryPath, gameModuleBuildDirectoryPath, "", CMakeHelper::Target::Editor);
	}

	/// @brief
	/// @return
	bool Project::BuildGameModule() const
	{
		Path gameModuleBuildDirectoryPath = GetCacheBuildDirPath() / "Editor";
		if (CMakeHelper::Build(gameModuleBuildDirectoryPath, "Release") == false)
		{
			return false;
		}
		// What it links now (see CanLoadProjectModules)
		FileSystem::GetInstance()->WriteAllText(GetGameModuleRuntimeLibrariesStampPath(), GetRuntimeLibrariesStamp());
		return true;
	}

	/// @brief The game module is a runtime module (HOD_STARTUP_RUNTIME_MODULE, started after the engine ones), its editor library an editor
	/// module (HOD_STARTUP_EDITOR_MODULE, started after the engine ones)
	/// @return false (with an error) if the game module can't be loaded or started
	bool Project::LoadProjectModules()
	{
		if (_gameModule.Load() == false)
		{
			return false;
		}

		ModuleStartup startup = _gameModule.LoadFunction<ModuleStartup>(HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME);
		if (startup == nullptr)
		{
			OUTPUT_ERROR("No " HOD_RUNTIME_MODULE_STARTUP_FUNCTION_NAME " in the game module {} (see HOD_STARTUP_RUNTIME_MODULE)", _gameModule.GetPath());
			_gameModule.Unload();
			return false;
		}

		if (_projectModuleHost.Start(_name + "Game", startup) == false)
		{
			_gameModule.Unload();
			return false;
		}

		// Optional
		if (FileSystem::GetInstance()->Exists(_editorModule.GetPath()) && _editorModule.Load())
		{
			ModuleStartup editorStartup = _editorModule.LoadFunction<ModuleStartup>(HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME);
			if (editorStartup != nullptr)
			{
				if (_projectModuleHost.Start(_name + "Editor", editorStartup) == false)
				{
					_editorModule.Unload();
				}
			}
			else
			{
				OUTPUT_ERROR("No " HOD_EDITOR_MODULE_STARTUP_FUNCTION_NAME " in the editor module {} (see HOD_STARTUP_EDITOR_MODULE)", _editorModule.GetPath());
				_editorModule.Unload();
			}
		}
		return true;
	}

	/// @brief In the reverse order of LoadProjectModules: the editor module (what it registered), then the game module (what it started and
	/// registered), while their code is still loaded
	void Project::UnloadProjectModules()
	{
		_projectModuleHost.StopAll(); // The editor library, then the game module
		_editorModule.Unload();
		_gameModule.Unload();
	}

	/// @brief
	/// @return
	bool Project::ReloadProjectModules()
	{
		UnloadProjectModules();
		if (LoadProjectModules() == false)
		{
			return false;
		}

		_onModulesReloadedEvent.Emit();

		return true;
	}
}
