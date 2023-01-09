#include "Asset.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

namespace hod::editor
{
	/// @brief 
	Asset::Asset(const std::filesystem::path& path)
		: _path(path)
	{
		_name = _path.stem().string();
		_uid = UID::GenerateUID();
	}

	/// @brief 
	Asset::~Asset()
	{
	}

	/// @brief 
	/// @return 
	bool Asset::Load()
	{
		/*
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
		*/
		return true;
	}

	/// @brief 
	/// @return 
	bool Asset::Save()
	{
		rapidjson::Document document;
		document.SetObject();

		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		document.AddMember("Name", rapidjson::StringRef(_name.c_str(), _name.size()), allocator);
		document.AddMember("Type", rapidjson::StringRef(GetType(), strlen(GetType())), allocator);
		document.AddMember("UID", rapidjson::StringRef(_uid.ToString().c_str(), _uid.ToString().size()), allocator);

		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		document.Accept(writer);

		std::ofstream file;
		file.open(_path, std::ios::out | std::ios::trunc);
		file << sb.GetString();
		file.close();
		return true;
	}

	/// @brief 
	/// @return 
	const UID& Asset::GetUid() const
	{
		return _uid;
	}
}
