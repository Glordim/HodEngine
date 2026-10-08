#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(HOD_RHI_STATIC)
	#define HOD_RHI_API
#elif defined(HOD_RHI_EXPORT)
	#define HOD_RHI_API HOD_EXPORT
#else
	#define HOD_RHI_API HOD_IMPORT
#endif
