#include <stdlib.h>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
	#include <HodEngine/Application/DesktopApplication.hpp>
#elif defined(PLATFORM_ANDROID)
	#include <HodEngine/Application/AndroidApplication.hpp>
#else
	#error
#endif

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
int main(int argc, char** argv)
{
	hod::DesktopApplication desktopApplication;
	if (desktopApplication.Run(argc, argv) == false)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#elif defined(PLATFORM_ANDROID)
void android_main(android_app* androidApp)
{
	hod::AndroidApplication androidApplication;
	androidApplication.Run(androidApp);
}
#else
	#error
#endif
