#include "AssetDatabase.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

#include "HodEngine/Editor/src/Editor.h"

namespace hod::editor
{
	/// @brief 
	AssetDatabase::AssetDatabase()
	{
	}

	/// @brief 
	AssetDatabase::~AssetDatabase()
	{
	}

	/// @brief 
	/// @return 
	bool AssetDatabase::Init()
	{
		Editor::GetInstance()->GetOnProjectOpenedSignal().Connect();
	}
}
