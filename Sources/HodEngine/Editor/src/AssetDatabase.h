#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/Src/UID.h>

namespace hod::editor
{
	class Asset;

	/// @brief 
	class AssetDatabase
	{
	public:
								AssetDatabase();
								~AssetDatabase();

		bool					Init();

	private:

		std::map<UID, Asset*>	_uidToAssetMap;
	};
}
