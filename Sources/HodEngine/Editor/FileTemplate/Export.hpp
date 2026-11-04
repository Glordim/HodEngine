#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined([[MODULE_EXPORT]])
	#define [[MODULE_API]] HOD_EXPORT
#else
	#define [[MODULE_API]] HOD_IMPORT
#endif
