#include "HodEngine/Editor/Project.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"

#include <fstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Project)
	{
		core::Reflection::Property::Variable* name = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::String, offsetof(Project, _name), "Name");
		AddProperty(name);
	}

	_SingletonConstructor(Project)
	{

	}

	/// @brief 
	/// @param directory 
	/// @return 
	bool Project::Create(const std::filesystem::path& directory)
	{
		if (std::filesystem::create_directories(directory) == false)
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

		return Load();
	}

	/// @brief 
	/// @return 
	bool Project::Load()
	{
		core::Document document;
		core::DocumentReaderJson reader;
		reader.Read(document, _projectPath);

		_name = document.GetRootNode()["Name"].GetString();

		return true;
	}

	/// @brief 
	/// @return 
	bool Project::Save()
	{
		core::Document document;
		document.GetRootNode().AddChild("Name").SetString(_name);

		core::DocumentWriterJson writer;
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
}
