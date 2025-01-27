#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <fstream>
#include <format>
#include <functional>
#include <algorithm>

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/SystemInfo.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Editor/FileTemplate/Generated/CMakeLists.txt.hpp"
#include "HodEngine/Editor/FileTemplate/Generated/Module.hpp.hpp"
#include "HodEngine/Editor/FileTemplate/Generated/Module.cpp.hpp"
#include "HodEngine/Editor/FileTemplate/Generated/Component.hpp.hpp"
#include "HodEngine/Editor/FileTemplate/Generated/Component.cpp.hpp"
#include "HodEngine/Editor/MissingGameModuleModal.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"

// todo move ?
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
//

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Project, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Project::_name, "_name");
		AddPropertyT(reflectionDescriptor, &Project::_startupScene, "_startupScene");
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

		if (CreateMinimalSourceForModule(directory) == false)
		{
			return false;
		}

		return Save();
	}

	/// @brief 
	/// @return 
	bool Project::CreateMinimalSourceForModule(const std::filesystem::path& directory)
	{
		std::filesystem::path sourcesDirPath = directory / "Sources";
		std::filesystem::path sourcesComponentsDirPath = sourcesDirPath / "Components";
		if (std::filesystem::exists(sourcesDirPath) == false && std::filesystem::create_directory(sourcesDirPath) == false)
		{
			return false;
		}
		if (std::filesystem::exists(sourcesComponentsDirPath) == false && std::filesystem::create_directory(sourcesComponentsDirPath) == false)
		{
			return false;
		}

		std::function<bool(const std::filesystem::path&, std::string_view, const std::string&)> writeFileFunc = [](const std::filesystem::path& path, std::string_view contentIn, const std::string& projectName)
		{
			std::string content(contentIn);

			constexpr std::string_view projectNameTag = "[[PROJECT_NAME]]";
			size_t replaceIndex = content.find(projectNameTag);
			while (replaceIndex != std::string::npos)
			{
				content.replace(replaceIndex, projectNameTag.size(), projectName);
				replaceIndex = content.find(projectNameTag);
			}

			std::string projectExport = projectName + "_EXPORT";
			std::transform(projectExport.begin(), projectExport.end(), projectExport.begin(), [](char c) { return std::toupper(c); });
			constexpr std::string_view projectExportTag = "[[PROJECT_EXPORT]]";
			replaceIndex = content.find(projectExportTag);
			while (replaceIndex != std::string::npos)
			{
				content.replace(replaceIndex, projectExportTag.size(), projectExport);
				replaceIndex = content.find(projectExportTag);
			}

			std::string api = projectName + "_API";
			std::transform(api.begin(), api.end(), api.begin(), [](char c) { return std::toupper(c); });
			constexpr std::string_view apiTag = "[[API]]";
			replaceIndex = content.find(apiTag);
			while (replaceIndex != std::string::npos)
			{
				content.replace(replaceIndex, apiTag.size(), api);
				replaceIndex = content.find(apiTag);
			}

			try
			{
				std::ofstream fileStream;
				fileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
				fileStream.open(path, std::ios_base::trunc);
				fileStream.write(content.c_str(), content.size());
				fileStream.close();

				OUTPUT_MESSAGE("MinimalSourceForModule generated at {}", path.string().c_str());

				return true;
			}
			catch (const std::ios_base::failure& e)
			{
				OUTPUT_ERROR("Failed to generate MinimalSourceForModule at {} : {}", path.string().c_str(), e.what());
				return false; 
			}
		};

		if (writeFileFunc(sourcesDirPath / (_name + ".hpp"), Module_hpp, _name) == false)
		{
			return false;
		}

		if (writeFileFunc(sourcesDirPath / (_name + ".cpp"), Module_cpp, _name) == false)
		{
			return false;
		}

		if (writeFileFunc(sourcesComponentsDirPath / "MyFirstComponent.hpp", Component_hpp, _name) == false)
		{
			return false;
		}

		if (writeFileFunc(sourcesComponentsDirPath / "MyFirstComponent.cpp", Component_cpp, _name) == false)
		{
			return false;
		}

		return true;		
	}

	/// @brief 
	/// @param projectPath 
	/// @return 
	bool Project::Open(const std::filesystem::path& projectPath)
	{
		if (std::filesystem::exists(projectPath) == false)
		{
			OUTPUT_ERROR("Project::Open fail: {} doesn't exist", projectPath.string());
			return false;
		}

		if (FileSystem::SetWorkingDirectory(projectPath.parent_path()) == false)
		{
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

		_gameModule.Init(_projectPath.parent_path() / "build" / "Release" / _name, true);
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
	const std::string Project::GetName() const
	{
		return _name;
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
		std::string cmakeLists(CMakeLists_txt);
		
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
		constexpr std::string_view enginePathTag = "[[ENGINE_PATH]]";
		size_t replaceIndex = cmakeLists.find(enginePathTag);
		while (replaceIndex != std::string::npos)
		{
			cmakeLists.replace(replaceIndex, enginePathTag.size(), enginePath);
			replaceIndex = cmakeLists.find(enginePathTag);
		}

		constexpr std::string_view projectNameTag = "[[PROJECT_NAME]]";
		replaceIndex = cmakeLists.find(projectNameTag);
		while (replaceIndex != std::string::npos)
		{
			cmakeLists.replace(replaceIndex, projectNameTag.size(), _name);
			replaceIndex = cmakeLists.find(projectNameTag);
		}

		std::string projectExport = _name + "_EXPORT";
		std::transform(projectExport.begin(), projectExport.end(), projectExport.begin(), [](char c) { return std::toupper(c); });
		constexpr std::string_view projectExportTag = "[[PROJECT_EXPORT]]";
		replaceIndex = cmakeLists.find(projectExportTag);
		while (replaceIndex != std::string::npos)
		{
			cmakeLists.replace(replaceIndex, projectExportTag.size(), projectExport);
			replaceIndex = cmakeLists.find(projectExportTag);
		}

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

		const char* config = "Release";
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
