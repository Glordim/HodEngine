#pragma once
#include <HodEngine/HodEngine.hpp>

#ifdef HOD_UI2_EDITOR_EXPORT
	#define HOD_UI2_EDITOR_API HOD_EXPORT
#else
	#define HOD_UI2_EDITOR_API HOD_IMPORT
#endif
