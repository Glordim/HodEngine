#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <functional>

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/SystemInfo.hpp"
#include "FileTemplates/CMakeLists.txt.hpp"
#include "FileTemplates/Component.cpp.hpp"
#include "FileTemplates/Component.hpp.hpp"
#include "FileTemplates/Module.cpp.hpp"
#include "FileTemplates/Module.hpp.hpp"
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

	_SingletonConstructor(Project) {}

	/// @brief
	/// @param directory
	/// @return
	bool Project::Create(const Path& directory)
	{
		if (FileSystem::GetInstance()->Exists(directory) == false && FileSystem::GetInstance()->CreateDirectories(directory) == false)
		{
			return false;
		}

		_projectPath = directory / (directory.Filename().GetString() + ".hod");
		_name = _projectPath.Stem().GetString();

		if (CreateMinimalSourceForModule(directory) == false)
		{
			return false;
		}

		return Save();
	}

	/// @brief
	/// @return
	bool Project::CreateMinimalSourceForModule(const Path& directory)
	{
		Path sourcesDirPath = directory / "Sources";
		Path sourcesComponentsDirPath = sourcesDirPath / "Components";
		if (FileSystem::GetInstance()->Exists(sourcesDirPath) == false && FileSystem::GetInstance()->CreateDirectories(sourcesDirPath) == false)
		{
			return false;
		}
		if (FileSystem::GetInstance()->Exists(sourcesComponentsDirPath) == false && FileSystem::GetInstance()->CreateDirectories(sourcesComponentsDirPath) == false)
		{
			return false;
		}

		std::function<bool(const Path&, std::string_view, const String&)> writeFileFunc = [](const Path& path, std::string_view contentIn, const String& projectName)
		{
			String content(contentIn);

			constexpr std::string_view projectNameTag = "[[PROJECT_NAME]]";
			size_t                     replaceIndex = content.Find(projectNameTag);
			while (replaceIndex != String::Npos)
			{
				content.Replace(replaceIndex, projectNameTag.size(), projectName);
				replaceIndex = content.Find(projectNameTag);
			}

			String projectExport = projectName + "_EXPORT";
			std::transform(projectExport.begin(), projectExport.end(), projectExport.begin(), [](char c) { return std::toupper(c); });
			constexpr std::string_view projectExportTag = "[[PROJECT_EXPORT]]";
			replaceIndex = content.Find(projectExportTag);
			while (replaceIndex != String::Npos)
			{
				content.Replace(replaceIndex, projectExportTag.size(), projectExport);
				replaceIndex = content.Find(projectExportTag);
			}

			String api = projectName + "_API";
			std::transform(api.begin(), api.end(), api.begin(), [](char c) { return std::toupper(c); });
			constexpr std::string_view apiTag = "[[API]]";
			replaceIndex = content.Find(apiTag);
			while (replaceIndex != String::Npos)
			{
				content.Replace(replaceIndex, apiTag.size(), api);
				replaceIndex = content.Find(apiTag);
			}

			try
			{
				std::ofstream fileStream;
				fileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
				fileStream.open(path.CStr(), std::ios_base::trunc);
				fileStream.write(content.CStr(), content.Size());
				fileStream.close();

				OUTPUT_MESSAGE("MinimalSourceForModule generated at {}", path);

				return true;
			}
			catch (const std::ios_base::failure& e)
			{
				OUTPUT_ERROR("Failed to generate MinimalSourceForModule at {} : {}", path, e.what());
				return false;
			}
		};

		if (writeFileFunc(sourcesDirPath / (_name + ".hpp").CStr(), Module_hpp, _name) == false)
		{
			return false;
		}

		if (writeFileFunc(sourcesDirPath / (_name + ".cpp").CStr(), Module_cpp, _name) == false)
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

		_assetDirPath = _projectPath.ParentPath() / "Assets";
		if (FileSystem::GetInstance()->Exists(_assetDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_assetDirPath) == false)
		{
			return false;
		}

		_resourceDirPath = _projectPath.ParentPath() / "Generated" / "Resources";
		if (FileSystem::GetInstance()->Exists(_resourceDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_resourceDirPath) == false)
		{
			return false;
		}

		_thumbnailDirPath = _projectPath.ParentPath() / "Generated" / "Thumbnails";
		if (FileSystem::GetInstance()->Exists(_thumbnailDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_thumbnailDirPath) == false)
		{
			return false;
		}

		_buildsDirPath = _projectPath.ParentPath() / "Builds";
		if (FileSystem::GetInstance()->Exists(_buildsDirPath) == false && FileSystem::GetInstance()->CreateDirectories(_buildsDirPath) == false)
		{
			return false;
		}

		ResourceManager::GetInstance()->SetResourceDirectory(_resourceDirPath);

		_gameModule.Init(_projectPath.ParentPath() / "build" / "Release" / _name.CStr(), true);
		FileSystem::GetInstance()->CreateDirectories(_gameModule.GetPath().ParentPath());
		if (_gameModuleFileSystemWatcher.Init(_gameModule.GetPath(), nullptr, nullptr, [this](const Path&) { _gameModule.Reload(); }, nullptr))
		{
			_gameModuleFileSystemWatcher.RegisterUpdateJob();
		}

		return Load();
	}

	/// @brief
	/// @return
	bool Project::Load()
	{
		Document           document;
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

	/// @brief
	/// @return
	const Path& Project::GetThumbnailDirPath() const
	{
		return _thumbnailDirPath;
	}

	/// @brief
	/// @return
	const Path& Project::GetBuildsDirPath() const
	{
		return _buildsDirPath;
	}

	/// @brief
	/// @return
	bool Project::HasGameModule() const
	{
		return FileSystem::GetInstance()->Exists(_gameModule.GetPath());
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
		String cmakeLists(CMakeLists_txt);

		String enginePath = FileSystem::GetExecutablePath().ParentPath().ParentPath().ParentPath().GetString().CStr(); // todo...
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
		size_t                     replaceIndex = cmakeLists.Find(enginePathTag);
		while (replaceIndex != String::Npos)
		{
			cmakeLists.Replace(replaceIndex, enginePathTag.size(), enginePath);
			replaceIndex = cmakeLists.Find(enginePathTag);
		}

		constexpr std::string_view projectNameTag = "[[PROJECT_NAME]]";
		replaceIndex = cmakeLists.Find(projectNameTag);
		while (replaceIndex != String::Npos)
		{
			cmakeLists.Replace(replaceIndex, projectNameTag.size(), _name);
			replaceIndex = cmakeLists.Find(projectNameTag);
		}

		String projectExport = _name + "_EXPORT";
		std::transform(projectExport.begin(), projectExport.end(), projectExport.begin(), [](char c) { return std::toupper(c); });
		constexpr std::string_view projectExportTag = "[[PROJECT_EXPORT]]";
		replaceIndex = cmakeLists.Find(projectExportTag);
		while (replaceIndex != String::Npos)
		{
			cmakeLists.Replace(replaceIndex, projectExportTag.size(), projectExport);
			replaceIndex = cmakeLists.Find(projectExportTag);
		}

		Path path = _projectPath.ParentPath() / "CMakeLists.txt";

		try
		{
			std::ofstream cmakeListFileStream;
			cmakeListFileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
			cmakeListFileStream.open(path.CStr(), std::ios_base::trunc);
			cmakeListFileStream.write(cmakeLists.CStr(), cmakeLists.Size());
			cmakeListFileStream.close();

			OUTPUT_MESSAGE("CMakeLists.txt generated at {}", path);

			return true;
		}
		catch (const std::ios_base::failure& e)
		{
			OUTPUT_ERROR("Failed to generate CMakeLists.txt at {} : {}", path, e.what());
			return false;
		}
	}

	/// @brief
	/// @param outputs
	/// @return
	bool Project::ConfigureGameModule() const
	{
		Path gameModuleSourceDirectoryPath = _projectPath.ParentPath();
		Path gameModuleBuildDirectoryPath = gameModuleSourceDirectoryPath / "build";

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

		Path toolchainPath = FileSystem::GetExecutablePath().ParentPath().ParentPath().ParentPath();

#if defined(PLATFORM_WINDOWS)
		// const char* generator = "Visual Studio 17 2022";
		// toolchainPath /= "CMake/Toolchains/Windows-Msvc-x64.cmake";
		const char* generator = "Ninja Multi-Config";
		toolchainPath /= "CMake/Toolchains/Windows-ClangCl-x64.cmake";
#elif defined(PLATFORM_MACOS)
		const char* generator = "Xcode";
		toolchainPath /= "CMake/Toolchains/MacOs-Clang-arm64.cmake";
#elif defined(PLATFORM_LINUX)
		const char* generator = "Ninja Multi-Config";
		toolchainPath /= "CMake/Toolchains/Linux-Clang-x64.cmake";
#else
	#error
#endif

		String arguments = fmt::format("-DCMAKE_TOOLCHAIN_FILE:FILEPATH={} --no-warn-unused-cli -B {} -S {} -G \"{}\"", toolchainPath, gameModuleBuildDirectoryPath,
		                               gameModuleSourceDirectoryPath, generator)
		                       .c_str();
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
		Path gameModuleSourceDirectoryPath = _projectPath.ParentPath();
		Path gameModuleBuildDirectoryPath = gameModuleSourceDirectoryPath / "build";

		const char* config = "Release";
		String      arguments = fmt::format("--build {} --config {} --parallel", gameModuleBuildDirectoryPath, config).c_str();
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
