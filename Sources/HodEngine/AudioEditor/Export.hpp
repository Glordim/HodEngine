#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_AUDIO_EDITOR_EXPORT
    #define HOD_AUDIO_EDITOR_API HOD_EXPORT
#else
    #define HOD_AUDIO_EDITOR_API HOD_IMPORT
#endif
