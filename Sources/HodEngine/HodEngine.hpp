#pragma once

#if defined(PLATFORM_WINDOWS)
    #define HOD_EXPORT __declspec(dllexport)
    #define HOD_IMPORT __declspec(dllimport)
#else
    #define HOD_EXPORT __attribute__((visibility("default")))
    #define HOD_IMPORT
#endif
