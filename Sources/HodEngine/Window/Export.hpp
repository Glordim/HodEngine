#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_WINDOW_STATIC)
	#define HOD_WINDOW_API
#elif defined(HOD_WINDOW_EXPORT)
	#define HOD_WINDOW_API HOD_EXPORT
#else
	#define HOD_WINDOW_API HOD_IMPORT
#endif
