#pragma once

#include <HodEngine/Application/PlatformApplication.h>

namespace hod::core
{
	class ArgumentParser;
}

/// @brief 
class EditorApplication : public PlatformApplication
{
	_SingletonOverride(EditorApplication)

public:

	bool			Init(const hod::core::ArgumentParser& argumentParser);
};

