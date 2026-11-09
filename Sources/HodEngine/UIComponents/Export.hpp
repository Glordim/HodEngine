#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_UI_COMPONENTS_STATIC)
	#define HOD_UI_COMPONENTS_API
#elif defined(HOD_UI_COMPONENTS_EXPORT)
	#define HOD_UI_COMPONENTS_API HOD_EXPORT
#else
	#define HOD_UI_COMPONENTS_API HOD_IMPORT
#endif
