#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputBucket.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
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
}
