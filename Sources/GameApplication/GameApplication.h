#pragma once

#include <HodEngine/Application/src/PlatformApplication.h>

namespace hod::core
{
	class ArgumentParser;
}

/// @brief 
class GameApplication : public PlatformApplication
{
public:

	bool			Init(const hod::core::ArgumentParser& argumentParser);
};

