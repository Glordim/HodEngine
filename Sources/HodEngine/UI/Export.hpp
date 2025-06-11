#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_UI_EXPORT
    #define HOD_UI_API HOD_EXPORT
#else
    #define HOD_UI_API HOD_IMPORT
#endif
