#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"

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

		PlatformOutput(type, content);
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
