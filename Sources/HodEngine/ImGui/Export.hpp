#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_IMGUI_STATIC)
	#define HOD_IMGUI_API
#elif defined(HOD_IMGUI_EXPORT)
	#define HOD_IMGUI_API HOD_EXPORT
#else
	#define HOD_IMGUI_API HOD_IMPORT
#endif
