#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Src/UID.h>

namespace hod::editor
{
	/// @brief 
	class Asset
	{
	public:
								Asset(const UID& uid, const std::filesystem::path& path);
								~Asset();

		bool					Load();
		bool					Save();

	private:


		UID						_uid;
		std::string				_name;
		std::filesystem::path	_path;
	};
}
