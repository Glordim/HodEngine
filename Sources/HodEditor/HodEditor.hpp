#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>

namespace hod
{
	class ArgumentParser;
}

/// @brief
class HodEditor : public PlatformApplication
{
	_SingletonOverride(HodEditor)

public:

	bool			Init(const hod::ArgumentParser& argumentParser);
	void			Terminate();
};
