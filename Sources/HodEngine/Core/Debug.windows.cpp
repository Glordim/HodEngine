#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"

#include <win32/dbghelp.h>

namespace hod
{
	bool IsDebuggerAttached()
	{
		return IsDebuggerPresent();
	}
}
