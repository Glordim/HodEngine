#pragma once

#ifdef HODENGINE_EXPORTS
    #define HODENGINE_API __declspec(dllexport)
#else
    #define HODENGINE_API __declspec(dllimport)
#endif
