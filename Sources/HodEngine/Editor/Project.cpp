#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/ResourceManager.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <fstream>
#include <format>

#include "HodEngine/Core/FileSystem.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/SystemInfo.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Editor/CMakeProjectTemplate/CMakeLists.txt.h"
#include "HodEngine/Editor/MissingGameModuleModal.hpp"

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
		ADD_PROPERTY(Project, _startupScene);
	}

	_SingletonConstructor(Project)
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
		if (std::filesystem::exists(projectPath) == false)
		{
			// todo
			return false;
		}

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

		_buildsDirPath = _projectPath.parent_path() / "Builds";
		if (std::filesystem::exists(_buildsDirPath) == false && std::filesystem::create_directories(_buildsDirPath) == false)
		{
			return false;
		}

		ResourceManager::GetInstance()->SetResourceDirectory(_resourceDirPath);

		_gameModule.Init(_projectPath.parent_path() / "build" / STRINGIZE_VALUE_OF(HOD_CONFIG) / "Game", true);
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

		Serializer::Deserialize(*this, document.GetRootNode());

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
		_startupScene = asset->GetMeta()._uid;
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
	const std::filesystem::path& Project::GetBuildsDirPath() const
	{
		return _buildsDirPath;
	}

	/// @brief 
	/// @return 
	bool Project::HasGameModule() const
	{
		return std::filesystem::exists(_gameModule.GetPath());
	}

	/// @brief 
	/// @return 
	std::filesystem::path Project::GetGameModulePath() const
	{
		return _gameModule.GetPath();
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

		std::string enginePath = FileSystem::GetExecutablePath().parent_path().parent_path().parent_path().string(); // todo...
		
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

		std::filesystem::path path = _projectPath.parent_path() / "CMakeLists.txt";

		try
		{
			std::ofstream cmakeListFileStream;
			cmakeListFileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
			cmakeListFileStream.open(path, std::ios_base::trunc);
			cmakeListFileStream.write(cmakeLists.c_str(), cmakeLists.size());
			cmakeListFileStream.close();

			OUTPUT_MESSAGE("CMakeLists.txt generated at {}", path.string().c_str());

			return true;
		}
		catch (const std::ios_base::failure& e)
		{
			OUTPUT_ERROR("Failed to generate CMakeLists.txt at {} : {}", path.string().c_str(), e.what());
			return false; 
		}
	}

	/// @brief 
	/// @param outputs 
	/// @return 
	bool Project::ConfigureGameModule() const
	{
		std::filesystem::path gameModuleSourceDirectoryPath = _projectPath.parent_path();
		std::filesystem::path gameModuleBuildDirectoryPath = gameModuleSourceDirectoryPath / "build";
		
		try
		{
			if (std::filesystem::exists(gameModuleBuildDirectoryPath) == false)
			{
				std::filesystem::create_directory(gameModuleBuildDirectoryPath);
				OUTPUT_MESSAGE("Create Build directory at {}", gameModuleBuildDirectoryPath.string());
			}
			else
			{
				OUTPUT_MESSAGE("Build directory already exist at {}", gameModuleBuildDirectoryPath.string());
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_MESSAGE("Failed to create Build directory at {} : ", gameModuleBuildDirectoryPath.string(), e.what());
			return false;
		}
		
		#if defined(PLATFORM_WINDOWS)
		const char* generator = "Visual Studio 17 2022";
		#elif defined(PLATFORM_MACOS)
		const char* generator = "Xcode";
		#elif defined(PLATFORM_LINUX)
		const char* generator = "Ninja Multi-Config";
		#else
			#error
		#endif

		std::string arguments = std::format("-B {} -S {} -G \"{}\"", gameModuleBuildDirectoryPath.string(), gameModuleSourceDirectoryPath.string(), generator);
		OUTPUT_MESSAGE("Execute: {} {}", "cmake", arguments);
		if (Process::Create("cmake", arguments, false) == false)
		{
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	bool Project::BuildGameModule() const
	{
		std::filesystem::path gameModuleSourceDirectoryPath = _projectPath.parent_path();
		std::filesystem::path gameModuleBuildDirectoryPath = gameModuleSourceDirectoryPath / "build";

		const char* config = STRINGIZE_VALUE_OF(HOD_CONFIG);
		std::string arguments = std::format("--build {} --config {} -j {}", gameModuleBuildDirectoryPath.string(), config, SystemInfo::GetLogicalCoreCount());
		OUTPUT_MESSAGE("Execute: {} {}", "cmake", arguments);
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
