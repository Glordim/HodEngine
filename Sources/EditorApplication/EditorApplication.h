#pragma once

#include <HodEngine/Application/PlatformApplication.h>

namespace hod
{
	class ArgumentParser;
}

/// @brief 
class EditorApplication : public PlatformApplication
{
	_SingletonOverride(EditorApplication)

public:

	bool			Init(const hod::ArgumentParser& argumentParser);
};

