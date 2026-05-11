#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"

#include <cerrno>
#include <cstring>

namespace hod::inline core
{
	const char* OS::GetErrnoMessage()
	{
		return strerror(errno);
	}
}
