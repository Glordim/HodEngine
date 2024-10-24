#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Concurency/SpinLock.hpp"

namespace hod
{
	/// @brief 
	void SpinLock::Lock()
	{
		while (_flag.test_and_set(std::memory_order_acquire))
		{
			// Spin
		}
	}

	/// @brief 
	void SpinLock::Unlock()
	{
		_flag.clear(std::memory_order_release);
	}
} 
