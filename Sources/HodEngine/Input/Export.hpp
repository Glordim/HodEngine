#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_INPUT_STATIC)
	#define HOD_INPUT_API
#elif defined(HOD_INPUT_EXPORT)
	#define HOD_INPUT_API HOD_EXPORT
#else
	#define HOD_INPUT_API HOD_IMPORT
#endif
