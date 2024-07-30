#include "HodEngine/Core/Output.hpp"

#include <cstdio>
#include <cstdarg>

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

namespace hod
{
	const char* Output::_typeNames[Type::Count] = {
		"Message",
		"Warning",
		"Error",
		"Assert"
	};

	/// @brief 
	/// @param type 
	/// @param content 
	Output::Output(Type type, const std::string_view& content)
	: _type(type)
	, _content(content)
	{

	}

	/// @brief 
	void OutputBucket::Clear()
	{
		_outputs.clear();
	}

	/// @brief 
	/// @return 
	std::mutex& OutputBucket::GetLock() const
	{
		return _lock;
	}

	/// @brief 
	/// @return 
	const std::vector<Output>& OutputBucket::GetOutputs() const
	{
		return _outputs;
	}

	/// @brief 
	/// @param output 
	void OutputBucket::AddOutput(const Output& output)
	{
		_lock.lock();
		_outputs.push_back(output);
		_lock.unlock();
	}

	/// @brief 
	/// @param outputBucket 
	ScopedEnableOutputBucket::ScopedEnableOutputBucket(OutputBucket& outputBucket)
	: _outputBucket(outputBucket)
	{
		OutputService::PushBucket(_outputBucket);
	}

	/// @brief 
	ScopedEnableOutputBucket::~ScopedEnableOutputBucket()
	{
		OutputService::PopBucket();
	}

	thread_local std::vector<OutputBucket*> OutputService::_buckets;

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
			OutputDebugString(finalBuffer);
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

	/// @brief 
	/// @param type 
	/// @return 
	const char* Output::GetTypeName(Type type)
	{
		return _typeNames[type];
	}
}
