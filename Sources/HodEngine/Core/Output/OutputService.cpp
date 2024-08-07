#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

namespace hod
{
	static thread_local std::vector<OutputBucket*> _buckets;

	/// @brief 
	/// @param fileName 
	/// @param lineNumber 
	/// @param type 
	/// @param format 
	/// @param  
	void OutputService::AddOutput(const char* fileName, int lineNumber, Output::Type type, const char* format)
	{
		for (OutputBucket* bucket : _buckets)
		{
			bucket->AddOutput(Output(type, format));
		}

		std::string content = std::format("{}({}) : {} : {}\n", fileName, lineNumber, Output::GetTypeName(type), format);

		if (type == Output::Type::Message)
		{
			fputs(content.c_str(), stdout);
		}
		else
		{
			fputs(content.c_str(), stderr);
		}

#if defined(PLATFORM_WINDOWS)
		if (IsDebuggerPresent() == TRUE)
		{
			OutputDebugString(content.c_str());
		}
#endif
	}

	/// @brief 
	/// @param bucket 
	void OutputService::PushBucket(OutputBucket& bucket)
	{
		_buckets.push_back(&bucket);
	}

	/// @brief 
	void OutputService::PopBucket()
	{
		_buckets.pop_back();
	}
}
