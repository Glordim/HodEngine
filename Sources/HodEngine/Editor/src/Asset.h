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
								Asset(const std::filesystem::path& path);
								~Asset();

		bool					Load();
		bool					Save();

		const UID&				GetUid() const;

	private:

		UID						_uid;
		std::string				_name;
		std::filesystem::path	_path;
	};
}
