#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_RENDERER_STATIC)
	#define HOD_RENDERER_API
#elif defined(HOD_RENDERER_EXPORT)
	#define HOD_RENDERER_API HOD_EXPORT
#else
	#define HOD_RENDERER_API HOD_IMPORT
#endif
