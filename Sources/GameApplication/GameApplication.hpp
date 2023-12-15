#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>

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

