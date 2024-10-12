#pragma once

#include <HodEngine/HodEngine.hpp>

#if defined(PLATFORM_ANDROID)
	#if defined(HOD_APP_EXPORT)
		#define HOD_APP_EXPORT HOD_EXPORT
	#else
		#define HOD_APP_EXPORT HOD_IMPORT
	#endif
#else
	#define HOD_APP_EXPORT
#endif

extern "C"
{
	HOD_APP_EXPORT int HodMain(int argc, char** argv);
#if defined(PLATFORM_ANDROID)
	HOD_APP_EXPORT void android_main(struct android_app* app);
#endif
}
