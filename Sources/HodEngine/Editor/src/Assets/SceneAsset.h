#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/Src/UID.h>

#include "../Asset.h"

namespace hod::editor
{
	/// @brief 
	class SceneAsset : public Asset
	{
	public:
								SceneAsset(const std::filesystem::path& path);
								~SceneAsset();

		const char*				GetType() const override { return "Scene"; }
	};
}
