#include "Project.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

namespace hod::editor
{
	/// @brief 
	Project::Project(const std::filesystem::path& projectPath)
		: _projectPath(projectPath)
	{
		_name = _projectPath.stem().string();

		_assetDirPath = _projectPath.parent_path();
		_assetDirPath.append("Assets");

		std::filesystem::create_directory(_assetDirPath);
	}

	/// @brief 
	Project::~Project()
	{
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
		buffer[size] = '\0';

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
	const std::filesystem::path& Project::GetAssetDirPath() const
	{
		return _assetDirPath;
	}
}
