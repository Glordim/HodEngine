#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_RENDERER_EXPORT
    #define HOD_RENDERER_API HOD_EXPORT
#else
    #define HOD_RENDERER_API HOD_IMPORT
#endif
