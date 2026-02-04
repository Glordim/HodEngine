#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
	static thread_local Vector<OutputBucket*> _buckets;

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

		String content = fmt::format("{}({}) : {} : {}\n", fileName, lineNumber, Output::GetTypeName(type), format).c_str();

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
		_buckets.PopBack();
	}
}
