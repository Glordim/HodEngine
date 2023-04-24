#include "Project.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

namespace hod::editor
{
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

		return Load();
	}

	/// @brief 
	/// @return 
	bool Project::Load()
	{
		std::ifstream file;
		file.open(_projectPath, std::ios::in);
		file.seekg(0, std::ios::end);
		
		int size = (int)file.tellg();
		char* buffer = new char[size + 1];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		buffer[size] == '\0';
		for (int i = size - 1; i >= 0; --i)
		{
			if (buffer[i] == '\0' || buffer[i] == 4)
			{
				buffer[i] = '\0';
				break;
			}
		}

		rapidjson::Document document;
		document.Parse(buffer);

		_name = document["Name"].GetString();

		delete[] buffer;
		return true;
	}

	/// @brief 
	/// @return 
	bool Project::Save()
	{
		rapidjson::Document document;
		document.SetObject();

		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		document.AddMember("Name", rapidjson::StringRef(_name.c_str(), _name.size()), allocator);
		//document["Name"].SetString(_name.c_str(), _name.size());

		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		document.Accept(writer);

		std::ofstream file;
		file.open(_projectPath, std::ios::out | std::ios::trunc);
		file << sb.GetString();
		file.close();
		return true;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetProjectPath()
	{
		return _projectPath;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Project::GetAssetDirPath()
	{
		return _assetDirPath;
	}
}
