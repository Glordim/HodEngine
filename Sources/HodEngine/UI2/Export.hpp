#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_UI2_STATIC)
	#define HOD_UI2_API
#elif defined(HOD_UI2_EXPORT)
	#define HOD_UI2_API HOD_EXPORT
#else
	#define HOD_UI2_API HOD_IMPORT
#endif
