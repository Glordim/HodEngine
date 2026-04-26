#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_APPLICATION_STATIC)
	#define HOD_APPLICATION_API
#elif defined(HOD_APPLICATION_EXPORT)
	#define HOD_APPLICATION_API HOD_EXPORT
#else
	#define HOD_APPLICATION_API HOD_IMPORT
#endif
