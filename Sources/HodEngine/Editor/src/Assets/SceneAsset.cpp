#include "SceneAsset.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

namespace hod::editor
{
	/// @brief 
	SceneAsset::SceneAsset(const std::filesystem::path& path)
		: Asset(path)
	{
	}

	/// @brief 
	SceneAsset::~SceneAsset()
	{
	}
}
