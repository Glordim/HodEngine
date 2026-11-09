#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_AUDIO_COMPONENTS_STATIC)
	#define HOD_AUDIO_COMPONENTS_API
#elif defined(HOD_AUDIO_COMPONENTS_EXPORT)
	#define HOD_AUDIO_COMPONENTS_API HOD_EXPORT
#else
	#define HOD_AUDIO_COMPONENTS_API HOD_IMPORT
#endif
