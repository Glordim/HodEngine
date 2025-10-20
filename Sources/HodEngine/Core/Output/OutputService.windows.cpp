#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <Windows.h>

namespace hod
{
	/// @brief
	/// @param type
	/// @param content
	void OutputService::PlatformOutput(Output::Type type, const String& content)
	{
		if (type == Output::Type::Message)
		{
			fputs(content.CStr(), stdout);
		}
		else
		{
			fputs(content.CStr(), stderr);
		}

		if (IsDebuggerPresent() == TRUE)
		{
			OutputDebugString(content.CStr());
		}
	}
}
