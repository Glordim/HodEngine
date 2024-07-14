#pragma once

#if defined(PLATFORM_WINDOWS)
    #ifdef HOD_EXPORT
        #define HOD_API __declspec(dllexport)
    #else
        #define HOD_API __declspec(dllimport)
    #endif
#else
    #ifdef HOD_EXPORT
        #define HOD_API __attribute__((visibility("default")))
    #else
        #define HOD_API
    #endif
#endif
