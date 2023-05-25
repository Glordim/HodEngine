#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/UID.h>
#include "HodEngine/Core/Reflection/ReflectionMacros.h"

namespace hod::editor
{
	// TODO embed in Asset when reflection support it
	struct Meta
	{
		REFLECTED_CLASS(Meta);

	public:

		UID	_uid;
	};

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

		Meta					_meta;

		std::string				_name;
		std::filesystem::path	_path;
	};
}
