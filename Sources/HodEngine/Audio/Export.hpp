#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_AUDIO_STATIC)
	#define HOD_AUDIO_API
#elif defined(HOD_AUDIO_EXPORT)
	#define HOD_AUDIO_API HOD_EXPORT
#else
	#define HOD_AUDIO_API HOD_IMPORT
#endif
