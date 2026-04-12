#include <HodEngine/HodEngine.hpp>
#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>

#if defined([[PROJECT_EXPORT]])
	#define [[API]] HOD_EXPORT
#else
	#define [[API]] HOD_IMPORT
#endif

HOD_DECLARE_MODULE([[PROJECT_NAME]], [[API]])
