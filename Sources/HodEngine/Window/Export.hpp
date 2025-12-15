#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_WINDOW_EXPORT
	#define HOD_WINDOW_API HOD_EXPORT
#else
	#define HOD_WINDOW_API HOD_IMPORT
#endif
