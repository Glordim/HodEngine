#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_MATH_STATIC)
	#define HOD_MATH_API
#elif defined(HOD_MATH_EXPORT)
	#define HOD_MATH_API HOD_EXPORT
#else
	#define HOD_MATH_API HOD_IMPORT
#endif
