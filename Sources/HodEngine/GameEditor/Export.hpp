#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_GAME_EDITOR_EXPORT
    #define HOD_GAME_EDITOR_API HOD_EXPORT
#else
    #define HOD_GAME_EDITOR_API HOD_IMPORT
#endif
