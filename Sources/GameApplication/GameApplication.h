#pragma once

#include <HodEngine/Application/PlatformApplication.h>

namespace hod
{
	class ArgumentParser;
}

/// @brief 
class GameApplication : public PlatformApplication
{
public:

	bool			Init(const hod::ArgumentParser& argumentParser);
};

