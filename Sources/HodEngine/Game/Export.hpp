#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_GAME_STATIC)
	#define HOD_GAME_API
#elif defined(HOD_GAME_EXPORT)
	#define HOD_GAME_API HOD_EXPORT
#else
	#define HOD_GAME_API HOD_IMPORT
#endif
