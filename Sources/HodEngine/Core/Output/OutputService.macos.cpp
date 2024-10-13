#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"

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
	}
}
