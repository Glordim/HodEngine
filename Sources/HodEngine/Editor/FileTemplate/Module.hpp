#include <HodEngine/HodEngine.hpp>

#if defined([[PROJECT_EXPORT]])
	#define [[API]] HOD_EXPORT
#else
	#define [[API]] HOD_IMPORT
#endif

extern "C"
{
	[[API]] int Init();
	[[API]] int Clean();
}
