#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_PHYSICS_EDITOR_EXPORT
    #define HOD_PHYSICS_EDITOR_API HOD_EXPORT
#else
    #define HOD_PHYSICS_EDITOR_API HOD_IMPORT
#endif
