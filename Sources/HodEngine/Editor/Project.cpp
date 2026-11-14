#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"
#include <HodEngine/Core/Module.hpp>
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"
#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"


#include "HodEngine/GameSystems/Resource/ResourceVariant.hpp"
#include <algorithm>
#include <filesystem>
#include <fmt/format.h>

#include "FileTemplates/CMakeListsEditor.txt.hpp"
#include "FileTemplates/CMakeListsRuntime.txt.hpp"
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
		AddPropertyT(reflectionDescriptor, &Project::_disabledPlugins, "_disabledPlugins");
	}

	_SingletonConstructor(Project) {}

	Project::~Project() = default; // Its plugin's modules are the editor's (see Editor::UnloadProjectPlugin)

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
		_disabledPlugins = createInfo._disabledPlugins;

		if (CreateMinimalSourceForModule(directory) == false)
		{
			return false;
		}

		return Save();
	}

	/// @brief
	/// @param name
	/// @return
	String Project::CheckName(const String& name)
	{
		if (name.Empty())
		{
			return "Name the project";
		}
		bool identifier = (name[0] >= '0' && name[0] <= '9') == false;
		for (char c : name)
		{
			identifier &= (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
		}
		if (identifier == false)
		{
			return String::Format("'%s' isn't a valid project name: letters, digits and '_' only, not starting with a digit", name.CStr());
		}

		String macroPrefix = name;
		std::transform(macroPrefix.begin(), macroPrefix.end(), macroPrefix.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });
		if (macroPrefix == "HOD" || macroPrefix.StartsWith("HOD_"))
		{
			return String::Format("'%s' isn't a valid project name: its export macros would be the engine's (HOD_...)", name.CStr());
		}

		// Two libraries of the same name in a process: the loader gives the first to whoever imports the name (case insensitive on Windows)
		const Path engineDirectory = FileSystem::GetExecutablePath().ParentPath();
		for (const String& module : {GetRuntimeModuleName(name), GetEditorModuleName(name)})
		{
			if (FileSystem::GetInstance()->Exists(DynamicLibrary::GetFilePath(engineDirectory / module)))
			{
				return String::Format("'%s' isn't a valid project name: its module %s would have the name of a library of the engine", name.CStr(), module.CStr());
			}
		}
		return String();
	}

	/// @brief
	/// @param projectName
	/// @return
	String Project::GetRuntimeModuleName(const String& projectName)
	{
		return projectName;
	}

	/// @brief
	/// @param projectName
	/// @return
	String Project::GetEditorModuleName(const String& projectName)
	{
		return projectName + "Editor";
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
		if (String error = CheckName(directory.Filename().GetString()); error.Empty() == false)
		{
			return error;
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
	bool Project::BuildEngineModuleGraph(PluginGraph& moduleGraph)
	{
		Vector<PluginManifest> manifests;
		bool                   result = PluginGraph::LoadManifests(FileSystem::GetExecutablePath().ParentPath() / "Plugins", manifests);
		result &= moduleGraph.Build(manifests);
		return result;
	}

	/// @brief Write a module source file from a template, replacing [[MODULE_NAME]], [[MODULE_API]] and [[MODULE_EXPORT]]
	/// @param path
	/// @param templateContent
	/// @param moduleName ex: Circle
	/// @param macroPrefix ex: Circle (uppercased to give CIRCLE_API / CIRCLE_EXPORT)
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

	/// @brief Generate the minimal sources of the runtime and editor modules
	/// Layout expected by GenerateProjectCMakeLists: Code/<Name> and Code/<Name>Editor
	/// @param directory
	/// @return
	bool Project::CreateMinimalSourceForModule(const Path& directory)
	{
		// Runtime module (macro prefix gives <NAME>_EXPORT, matching GenerateProjectCMakeLists)
		String runtimeModuleName = GetRuntimeModuleName(_name);
		String runtimeMacroPrefix = runtimeModuleName;
		Path   runtimeDirPath = directory / "Code" / runtimeModuleName;
		Path   runtimeComponentsDirPath = runtimeDirPath / "Components";
		if (FileSystem::GetInstance()->Exists(runtimeComponentsDirPath) == false && FileSystem::GetInstance()->CreateDirectories(runtimeComponentsDirPath) == false)
		{
			return false;
		}

		// Editor module
		String editorModuleName = GetEditorModuleName(_name);
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
			{runtimeDirPath / "Export.hpp", Export_hpp, runtimeModuleName, runtimeMacroPrefix},
			{runtimeDirPath / "Module.cpp", Module_cpp, runtimeModuleName, runtimeMacroPrefix},
			{runtimeComponentsDirPath / "MyFirstComponent.hpp", Component_hpp, runtimeModuleName, runtimeMacroPrefix},
			{runtimeComponentsDirPath / "MyFirstComponent.cpp", Component_cpp, runtimeModuleName, runtimeMacroPrefix},
			{editorDirPath / "Export.hpp", Export_hpp, editorModuleName, editorMacroPrefix},
			{editorDirPath / "Module.cpp", EditorModule_cpp, editorModuleName, editorMacroPrefix},
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
		if (String error = CheckName(_name); error.Empty() == false)
		{
			OUTPUT_ERROR("Project::Open fail: {}", error);
			return false;
		}

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

		Path modulesDirectoryPath = GetCacheBuildDirPath() / "Editor" / "Output" / "platforms" / Path(CMakeHelper::GetCurrentPlatform()) /
		                            Path(CMakeHelper::GetCurrentAbi()) / "shared" / "bin";
		FileSystem::GetInstance()->CreateDirectories(modulesDirectoryPath);

		_modulesDirectoryPath = modulesDirectoryPath;
		_runtimeModuleLibraryPath = modulesDirectoryPath / GetRuntimeModuleName(_name);
		if (_runtimeModuleFileSystemWatcher.Init(GetRuntimeModulePath(), nullptr, nullptr, [this](const Path&) { _onModulesRebuiltEvent.Emit(); }, nullptr) == false)
		{
			return false;
		}

		_editorModuleLibraryPath = modulesDirectoryPath / GetEditorModuleName(_name);
		if (_editorModuleFileSystemWatcher.Init(DynamicLibrary::GetFilePath(_editorModuleLibraryPath), nullptr, nullptr, [this](const Path&) { _onModulesRebuiltEvent.Emit(); },
		                                        nullptr) == false)
		{
			return false;
		}

		if (Load() == false)
		{
			return false;
		}
		ResolvePlugins();
		return true;
	}

	/// @brief The engine plugins described next to the editor (see PluginManifest), without the ones the project disables, then the project's
	/// own plugin, last (see GetProjectPlugin)
	void Project::ResolvePlugins()
	{
		BuildEngineModuleGraph(_pluginGraph);
		_pluginGraph.Resolve(_disabledPlugins);

		PluginManifest projectPlugin;
		projectPlugin._name = _name;
		projectPlugin._description = "The project's code: its runtime module and its editor module";
		projectPlugin._runtimeModules.PushBack(GetRuntimeModuleName(_name));
		projectPlugin._editorModule = GetEditorModuleName(_name); // Optional (see Editor::LoadProjectPlugin)
		projectPlugin._modulesDirectory = _modulesDirectoryPath;
		projectPlugin._builtFromSources = true;
		_pluginGraph.AppendLast(projectPlugin);

		for (const String& error : _pluginGraph.GetErrors())
		{
			OUTPUT_ERROR("Modules: {}", error);
		}
		for (const PluginGraph::Plugin& plugin : _pluginGraph.GetPlugins())
		{
			if (plugin._enabled == false)
			{
				OUTPUT_MESSAGE("Plugin '{}' disabled: {}", plugin._manifest._name, plugin._disabledReason);
			}
		}
	}

	/// @brief
	/// @return
	const PluginGraph& Project::GetPluginGraph() const
	{
		return _pluginGraph;
	}

	/// @brief
	/// @return
	const PluginManifest* Project::GetProjectPlugin() const
	{
		const PluginGraph::Plugin* plugin = _pluginGraph.Find(_name);
		return plugin != nullptr && plugin->_needsAll ? &plugin->_manifest : nullptr;
	}

	/// @brief
	/// @return
	const Vector<String>& Project::GetDisabledPlugins() const
	{
		return _disabledPlugins;
	}

	/// @brief
	/// @param disabledPlugins
	/// @return
	bool Project::SetDisabledPlugins(const Vector<String>& disabledPlugins)
	{
		String error;
		if (VersionControl::GetInstance() != nullptr && VersionControl::GetInstance()->PrepareWrite(ArrayView<Path>(&_projectPath, 1), &error) == false)
		{
			OUTPUT_ERROR("Can't save {}: {}", _projectPath, error);
			return false;
		}
		Vector<String> previous = _disabledPlugins;
		_disabledPlugins = disabledPlugins;
		if (Save() == false)
		{
			_disabledPlugins = previous;
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	Path Project::GetRuntimeModulesStampPath() const
	{
		return GetCacheBuildDirPath() / "Editor" / "HodModules.txt";
	}

	/// @brief
	/// @return
	String Project::GetRuntimeModulesStamp() const
	{
		String stamp;
		for (const String& runtimeModule : _pluginGraph.GetEnabledRuntimeModules())
		{
			stamp += runtimeModule;
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
	bool Project::HasRuntimeModule() const
	{
		return FileSystem::GetInstance()->Exists(GetRuntimeModulePath());
	}

	/// @brief Try to load then unload the project modules, a module linked against outdated engine libraries fails to load
	/// @return
	bool Project::CanLoadProjectModules()
	{
		// Built with other plugins than the ones the project uses now: it links the runtime modules of the plugins it was built with
		String builtStamp;
		if (FileSystem::GetInstance()->Exists(GetRuntimeModulesStampPath()) == false ||
		    FileSystem::GetInstance()->ReadAllText(GetRuntimeModulesStampPath(), builtStamp) == false || builtStamp != GetRuntimeModulesStamp())
		{
			OUTPUT_MESSAGE("The project's runtime module was built with other plugins, it must be rebuilt");
			return false;
		}

		DynamicLibrary runtimeModule;
		runtimeModule.Init(_runtimeModuleLibraryPath, true); // A copy, as the editor loads it (see PluginManifest::_builtFromSources)
		if (runtimeModule.Load() == false)
		{
			return false;
		}
		// Built against an engine whose entry point had another name (see HOD_STARTUP_MODULE): it loads, but can't be started
		if (runtimeModule.LoadFunction<void*>(HOD_MODULE_STARTUP_FUNCTION_NAME) == nullptr)
		{
			OUTPUT_MESSAGE("The project's runtime module has no entry point " HOD_MODULE_STARTUP_FUNCTION_NAME " (built against another engine), it must be rebuilt");
			return false;
		}

		if (FileSystem::GetInstance()->Exists(DynamicLibrary::GetFilePath(_editorModuleLibraryPath)))
		{
			DynamicLibrary editorModule;
			editorModule.Init(_editorModuleLibraryPath, false);
			if (editorModule.Load() == false)
			{
				return false;
			}
		}
		return true; // Unloaded (their destructor)
	}

	/// @brief
	/// @return
	Path Project::GetRuntimeModulePath() const
	{
		return DynamicLibrary::GetFilePath(_runtimeModuleLibraryPath);
	}

	/// @brief
	/// @return
	bool Project::GenerateProjectCMakeLists() const
	{
		String projectName = _name;

		String projectRuntimeName = GetRuntimeModuleName(_name);
		String projectRuntimeExport = projectRuntimeName + "_EXPORT";
		std::transform(projectRuntimeExport.begin(), projectRuntimeExport.end(), projectRuntimeExport.begin(), [](char c) { return std::toupper(c); });

		// The runtime modules of the enabled plugins, dependencies first: linked by the project's runtime module, started in that order by an application build
		// (its CMakeLists turns the list into links and the source calling them: hod_link_engine_modules... in HodEngineConfig.cmake), loaded by the editor from their libraries
		String moduleList;
		for (const String& runtimeModule : _pluginGraph.GetEnabledRuntimeModules())
		{
			moduleList += runtimeModule;
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

		result &= CMakeHelper::GenerateCMakeLists(_codeDirPath / projectRuntimeName / "CMakeLists.txt", CMakeListsRuntime_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectRuntimeName},
													  {"[[PROJECT_EXPORT]]", projectRuntimeExport},
												  });

		String projectEditorName = GetEditorModuleName(_name);
		String projectEditorExport = _name + "_EDITOR_EXPORT";
		std::transform(projectEditorExport.begin(), projectEditorExport.end(), projectEditorExport.begin(), [](char c) { return std::toupper(c); });

		result &= CMakeHelper::GenerateCMakeLists(_codeDirPath / projectEditorName / "CMakeLists.txt", CMakeListsEditor_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectName},
													  {"[[PROJECT_EDITOR_NAME]]", projectEditorName},
													  {"[[PROJECT_EDITOR_EXPORT]]", projectEditorExport},
												  });

		result &= CMakeHelper::GenerateCMakeLists(_projectPath.ParentPath() / "CMakeLists.txt", CMakeListsRoot_txt,
		                                          {
													  {"[[PROJECT_NAME]]", projectName},
													  {"[[PROJECT_EDITOR_NAME]]", projectEditorName},
												  });

		return result;
	}

	/// @brief
	/// @param outputs
	/// @return
	bool Project::ConfigureProjectModules() const
	{
		Path sourceDirectoryPath = _projectPath.ParentPath();
		Path buildDirectoryPath = GetCacheBuildDirPath() / "Editor";

		try
		{
			if (FileSystem::GetInstance()->Exists(buildDirectoryPath) == false)
			{
				FileSystem::GetInstance()->CreateDirectories(buildDirectoryPath);
				OUTPUT_MESSAGE("Create Build directory at {}", buildDirectoryPath);
			}
			else
			{
				OUTPUT_MESSAGE("Build directory already exist at {}", buildDirectoryPath);
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_MESSAGE("Failed to create Build directory at {} : ", buildDirectoryPath, e.what());
			return false;
		}

		return CMakeHelper::Configure(sourceDirectoryPath, buildDirectoryPath, "", CMakeHelper::Target::Editor);
	}

	/// @brief
	/// @return
	bool Project::BuildProjectModules() const
	{
		Path buildDirectoryPath = GetCacheBuildDirPath() / "Editor";
		if (CMakeHelper::Build(buildDirectoryPath, "Release") == false)
		{
			return false;
		}
		// What it links now (see CanLoadProjectModules)
		FileSystem::GetInstance()->WriteAllText(GetRuntimeModulesStampPath(), GetRuntimeModulesStamp());
		return true;
	}

}
