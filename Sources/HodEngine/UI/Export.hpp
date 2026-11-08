#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_UI_STATIC)
	#define HOD_UI_API
#elif defined(HOD_UI_EXPORT)
	#define HOD_UI_API HOD_EXPORT
#else
	#define HOD_UI_API HOD_IMPORT
#endif
