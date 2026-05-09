#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_MAIN_STATIC)
	#define HOD_MAIN_API
#elif defined(HOD_MAIN_EXPORT)
	#define HOD_MAIN_API HOD_EXPORT
#else
	#define HOD_MAIN_API HOD_IMPORT
#endif
