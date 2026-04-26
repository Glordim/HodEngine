#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_PHYSICS_STATIC)
	#define HOD_PHYSICS_API
#elif defined(HOD_PHYSICS_EXPORT)
	#define HOD_PHYSICS_API HOD_EXPORT
#else
	#define HOD_PHYSICS_API HOD_IMPORT
#endif
