#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"

#include <android/log.h>

namespace hod
{
	/// @brief 
	/// @param type 
	/// @param content 
	void OutputService::PlatformOutput(Output::Type type, const std::string& content)
	{
		if (type == Output::Type::Message)
		{
			__android_log_print(ANDROID_LOG_INFO, "HodEngine", "%s", content.c_str());
		}
		else
		{
			__android_log_print(ANDROID_LOG_ERROR, "HodEngine", "%s", content.c_str());
		}
	}
}
