#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"

#include <Windows.h>

namespace hod
{
	/// @brief 
	/// @param type 
	/// @param content 
	void OutputService::PlatformOutput(Output::Type type, const std::string& content)
	{
		if (type == Output::Type::Message)
		{
			fputs(content.c_str(), stdout);
		}
		else
		{
			fputs(content.c_str(), stderr);
		}

		if (IsDebuggerPresent() == TRUE)
		{
			OutputDebugString(content.c_str());
		}
	}
}