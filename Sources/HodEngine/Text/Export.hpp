#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_TEXT_STATIC)
	#define HOD_TEXT_API
#elif defined(HOD_TEXT_EXPORT)
	#define HOD_TEXT_API HOD_EXPORT
#else
	#define HOD_TEXT_API HOD_IMPORT
#endif
