#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/ResourceManager.hpp"

#include <fstream>
#include <format>

#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/SystemInfo.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Editor/CMakeProjectTemplate/CMakeLists.txt.h"
#include "HodEngine/Editor/MissingGameModuleModal.hpp"
#include "HodEngine/Application/PlatformDialog.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"

// todo move ?
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
//

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Project, void)
	{
		ADD_PROPERTY(Project, _name);
	}

	_SingletonConstructor(Project)
	: _gameModule("Game", true)
	{

	}

	/// @brief 
	/// @param directory 
	/// @return 
	bool Project::Create(const std::filesystem::path& directory)
	{
		if (std::filesystem::exists(directory) == false && std::filesystem::create_directories(directory) == false)
		{
			return false;
		}

		_projectPath = directory / (directory.filename().string() + ".hod");
		_name = _projectPath.stem().string();

		return Save();
	}

	/// @brief 
	/// @param projectPath 
	/// @return 
	bool Project::Open(const std::filesystem::path& projectPath)
	{
		_projectPath = projectPath;
		_name = _projectPath.stem().string();

		_assetDirPath = _projectPath.parent_path() / "Assets";
		if (std::filesystem::exists(_assetDirPath) == false && std::filesystem::create_directory(_assetDirPath) == false)
		{
			return false;
		}

		_resourceDirPath = _projectPath.parent_path() / "Generated" / "Resources";
		if (std::filesystem::exists(_resourceDirPath) == false && std::filesystem::create_directories(_resourceDirPath) == false)
		{
			return false;
		}

		_thumbnailDirPath = _projectPath.parent_path() / "Generated" / "Thumbnails";
		if (std::filesystem::exists(_thumbnailDirPath) == false && std::filesystem::create_directories(_thumbnailDirPath) == false)
		{
			return false;
		}

		ResourceManager::GetInstance()->SetResourceDirectory(_resourceDirPath);

		_gameModule = Module(_projectPath.parent_path() / "build" / STRINGIZE_VALUE_OF(HOD_CONFIG) / "bin" / "Game", true);
		std::filesystem::create_directories(_gameModule.GetPath().parent_path());
		_gameModuleFileSystemWatcher.Init(_gameModule.GetPath(), nullptr, nullptr, [this](const std::filesystem::path&){ _gameModule.Reload(); }, nullptr);
		_gameModuleFileSystemWatcher.RegisterUpdateJob();

		return Load();
	}

	/// @brief 
	/// @return 
	bool Project::Load()
	{
		Document document;
		DocumentReaderJson reader;
		reader.Read(document, _projectPath);

		_name = document.GetRootNode()["Name"].GetString();

		return true;
	}

	/// @brief 
	/// @return 
	bool Project::Save()
	{
		Document document;
		document.GetRootNode().AddChild("Name").SetString(_name);

		DocumentWriterJson writer;
		writer.Write(document, _projectPath);

		return true;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetProjectPath() const
	{
		return _projectPath;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetAssetDirPath() const
	{
		return _assetDirPath;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetResourceDirPath() const
	{
		return _resourceDirPath;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetThumbnailDirPath() const
	{
		return _thumbnailDirPath;
	}

	/// @brief 
	/// @return 
	bool Project::HasGameModule() const
	{
		return std::filesystem::exists(_gameModule.GetPath());
	}

	/// @brief 
	/// @return 
	bool Project::GenerateGameModuleCMakeList() const
	{
		constexpr std::string_view replaceByEnginePath = "REPLACE_ME_BY_ENGINE_PATH";

		std::string cmakeLists(reinterpret_cast<const char*>(CMakeLists_txt), CMakeLists_txt_size);
		size_t replaceByEnginePathIndex = cmakeLists.find(replaceByEnginePath);
		if (replaceByEnginePathIndex == std::string::npos)
		{
			return false;
		}

		std::string enginePath = FileSystem::GetExecutablePath().parent_path().parent_path().parent_path().parent_path().string(); // todo...
		
#if defined(PLATFORM_WINDOWS)
		// CMakeLists require portable path
		for (char& c : enginePath)
		{
			if (c == '\\')
			{
				c = '/';
			}
        }
#endif

		cmakeLists.replace(replaceByEnginePathIndex, replaceByEnginePath.size(), enginePath);

		std::ofstream cmakeListFileStream(_projectPath.parent_path() / "CMakeLists.txt", std::ios_base::trunc);
		if (cmakeListFileStream.is_open() == false)
		{
			return false;
		}

		cmakeListFileStream.write(cmakeLists.c_str(), cmakeLists.size());
		return true;
	}

	/// @brief 
	/// @return 
	bool Project::BuildGameModule() const
	{
		std::filesystem::path gameModuleCMakeListsPath = _projectPath.parent_path() / "CMakeLists.txt";
		std::filesystem::path gameModuleBuildDirectoryPath = gameModuleCMakeListsPath.parent_path() / "build";
		
		if (std::filesystem::exists(gameModuleBuildDirectoryPath) == false)
		{
			std::filesystem::create_directory(gameModuleBuildDirectoryPath);
		}

		std::string arguments = std::format("-B {}", gameModuleBuildDirectoryPath.string());
		if (Process::Create("cmake", arguments, false) == false)
		{
			return false;
		}

		const char* config = STRINGIZE_VALUE_OF(HOD_CONFIG);
		arguments = std::format("--build {} --config {} -j {}", gameModuleBuildDirectoryPath.string(), config, SystemInfo::GetLogicalCoreCount());
		if (Process::Create("cmake", arguments, false) == false)
		{
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	bool Project::ReloadGameModule()
	{
		return _gameModule.Reload();
	}
}
