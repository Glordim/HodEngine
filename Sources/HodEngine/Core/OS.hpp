#pragma once
#include "HodEngine/Core/Export.hpp"

#include <string>

namespace hod
{
	class HOD_CORE_API OS
	{
	public:

#if defined(PLATFORM_WINDOWS)
		static std::string GetLastWin32ErrorMessage();
#endif
	};
}
