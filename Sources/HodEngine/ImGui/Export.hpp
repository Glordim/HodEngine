#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_IMGUI_EXPORT
    #define HOD_IMGUI_API HOD_EXPORT
#else
    #define HOD_IMGUI_API HOD_IMPORT
#endif
