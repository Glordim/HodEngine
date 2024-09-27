#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>

namespace hod
{
	class ArgumentParser;
}

/// @brief
class HodApplication : public PlatformApplication
{
	_SingletonOverride(HodApplication)

public:

	bool			Init(const hod::ArgumentParser& argumentParser);
	void			Terminate();
};
