#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_RENDERER_EDITOR_EXPORT
    #define HOD_RENDERER_EDITOR_API HOD_EXPORT
#else
    #define HOD_RENDERER_EDITOR_API HOD_IMPORT
#endif
