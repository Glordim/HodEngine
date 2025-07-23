#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_INPUT_EDITOR_EXPORT
    #define HOD_INPUT_EDITOR_API HOD_EXPORT
#else
    #define HOD_INPUT_EDITOR_API HOD_IMPORT
#endif
