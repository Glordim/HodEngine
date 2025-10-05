#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_APPLICATION_EXPORT
	#define HOD_APPLICATION_API HOD_EXPORT
#else
	#define HOD_APPLICATION_API HOD_IMPORT
#endif
