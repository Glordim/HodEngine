#pragma once

#include <HodEngine/HodEngine.hpp>

#if defined(HOD_APP_EXPORT)
	#define HOD_APP_EXPORT HOD_EXPORT
#else
	#define HOD_APP_EXPORT HOD_IMPORT
#endif

extern "C"
{
	HOD_APP_EXPORT int HodMain(int argc, char** argv);
	HOD_APP_EXPORT void android_main(struct android_app* app);
}
