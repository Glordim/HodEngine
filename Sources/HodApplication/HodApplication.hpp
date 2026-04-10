#pragma once

#include <HodEngine/Application/PlatformApplication.hpp>
#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>

namespace hod
{
	class ArgumentParser;
}

namespace hod::game
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

	hod::DynamicLibrary	_gameModule;
	hod::game::World*	_world = nullptr;
};
