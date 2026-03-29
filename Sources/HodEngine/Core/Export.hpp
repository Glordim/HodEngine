#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_CORE_STATIC)
	#define HOD_CORE_API 
#elif defined(HOD_CORE_EXPORT)
	#define HOD_CORE_API HOD_EXPORT
#else
	#define HOD_CORE_API HOD_IMPORT
#endif
