#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_GAME_SYSTEMS_EXPORT
    #define HOD_GAME_SYSTEMS_API HOD_EXPORT
#else
    #define HOD_GAME_SYSTEMS_API HOD_IMPORT
#endif
