#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_GAME_EXPORT
    #define HOD_GAME_API HOD_EXPORT
#else
    #define HOD_GAME_API HOD_IMPORT
#endif
