#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Core/FileSystem/Path.hpp"

#include <cstdint>
#include <initializer_list>
#include <string_view>
#include <utility>

namespace hod::inline editor
{
	class HOD_EDITOR_API CMakeHelper
	{
	public:
		enum class Target : uint8_t
		{
			Editor,
			Retail,
		};

		static std::string_view GetCurrentPlatform();
		static Path             GetRunningToolchainPath();
		static Path             GetInstalledToolchainPath();

		static Path             GetRunningCmakeDirPath();
		static Path             GetInstalledCmakeDirPath();

		static bool GenerateCMakeLists(const Path& outputPath, std::string_view templateContent,
		                               std::initializer_list<std::pair<std::string_view, std::string_view>> replacements);

		static bool Configure(const Path& sourceDir, const Path& buildDir, const Path& installDir, Target target);
		static bool Build(const Path& buildDir, const char* config);
		static bool Install(const Path& buildDir, const char* config);
	};
}
