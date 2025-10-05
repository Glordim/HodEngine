#pragma once
#include "HodEngine/Core/Export.hpp"

#include <atomic>

namespace hod
{
	class HOD_CORE_API SpinLock
	{
	public:
		SpinLock() = default;
		SpinLock(const SpinLock&) = delete;

		SpinLock& operator=(const SpinLock&) = delete;

		void Lock();
		void Unlock();

	private:
		std::atomic_flag _flag;
	};
}
