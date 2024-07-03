#pragma once

#ifdef HOD_EXPORT
    #define HOD_API __declspec(dllexport)
#else
    #define HOD_API __declspec(dllimport)
#endif
