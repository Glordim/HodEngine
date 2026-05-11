#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <cstdlib>

namespace hod::inline core
{
	bool OS::SetEnv(const char* name, const char* value)
	{
		if (setenv(name, value, 1) != 0)
		{
			OUTPUT_ERROR("OS::SetEnv: set {}={} fail ({})", name, value, OS::GetErrnoMessage());
			return false;
		}
		return true;
	}
}
