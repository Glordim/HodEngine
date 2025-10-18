#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_CORE_EXPORT
	#define HOD_CORE_API HOD_EXPORT
#else
	#define HOD_CORE_API HOD_IMPORT
#endif
