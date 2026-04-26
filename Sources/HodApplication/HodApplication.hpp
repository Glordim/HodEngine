#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>

#if !defined(HOD_GAME_MODULE_STATIC)
	#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>
#endif

namespace hod::inline core
{
	class ArgumentParser;
}

namespace hod::inline game
{
	class World;
}

/// @brief
class HodApplication : public PlatformApplication
{
	_SingletonOverride(HodApplication)

public:

	bool			Init(const hod::ArgumentParser& argumentParser);
	void			Terminate();

private:

#if !defined(HOD_GAME_MODULE_STATIC)
	hod::DynamicLibrary	_gameModule;
#endif
	hod::World*	_world = nullptr;
};
