#pragma once
#include "HodEngine/Core/Export.hpp"

#include <HodEngine/Core/Output/Output.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <mutex>

namespace hod
{
	/// @brief
	class HOD_CORE_API OutputBucket
	{
	public:
		OutputBucket() = default;
		OutputBucket(const OutputBucket&) = delete;
		OutputBucket(OutputBucket&&) = delete;
		~OutputBucket() = default;

		OutputBucket& operator=(const OutputBucket&) = delete;
		OutputBucket& operator=(OutputBucket&&) = delete;

	public:
		void Clear();

		std::mutex&           GetLock() const;
		const Vector<Output>& GetOutputs() const;
		void                  AddOutput(const Output& output);

	private:
		Vector<Output>     _outputs;
		mutable std::mutex _lock;
	};

	/// @brief
	class HOD_CORE_API ScopedEnableOutputBucket
	{
	public:
		ScopedEnableOutputBucket(OutputBucket& outputBucket);
		ScopedEnableOutputBucket(const ScopedEnableOutputBucket&) = delete;
		ScopedEnableOutputBucket(ScopedEnableOutputBucket&&) = delete;
		~ScopedEnableOutputBucket();

		ScopedEnableOutputBucket& operator=(const ScopedEnableOutputBucket&) = delete;
		ScopedEnableOutputBucket& operator=(ScopedEnableOutputBucket&&) = delete;

	private:
		OutputBucket& _outputBucket;
	};
}
