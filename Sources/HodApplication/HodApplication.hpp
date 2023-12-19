#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>

namespace hod
{
	class ArgumentParser;
}

/// @brief
class HodApplication : public PlatformApplication
{
public:

	bool			Init(const hod::ArgumentParser& argumentParser);
};
