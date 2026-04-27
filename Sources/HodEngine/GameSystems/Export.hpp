#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_GAME_SYSTEMS_STATIC)
	#define HOD_GAME_SYSTEMS_API
#elif defined(HOD_GAME_SYSTEMS_EXPORT)
	#define HOD_GAME_SYSTEMS_API HOD_EXPORT
#else
	#define HOD_GAME_SYSTEMS_API HOD_IMPORT
#endif
