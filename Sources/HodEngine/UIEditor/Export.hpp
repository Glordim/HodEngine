#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_UI_EDITOR_EXPORT
    #define HOD_UI_EDITOR_API HOD_EXPORT
#else
    #define HOD_UI_EDITOR_API HOD_IMPORT
#endif
