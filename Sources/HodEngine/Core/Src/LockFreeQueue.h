#pragma once

#include <stdint.h>
#include <atomic>

namespace hod
{
	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	template<typename Type, uint32_t Capacity>
	class LockFreeQueue
	{
	public:

						LockFreeQueue() = default;
						LockFreeQueue(const LockFreeQueue&) = delete;
						LockFreeQueue(LockFreeQueue&&) = delete;
						~LockFreeQueue() = default;

		LockFreeQueue&	operator = (const LockFreeQueue&) = delete;
		LockFreeQueue&	operator = (LockFreeQueue&&) = delete;

	public:

		bool			Push(const Type& data);
		bool			Pop(Type& data);

		uint32_t		GetCapacity() const;
		uint32_t		GetSize() const;

	private:

		Type					_values[Capacity];

		std::atomic<uint32_t>	_pushStartIndex = 0;
		std::atomic<uint32_t>	_pushEndIndex = 0;
		std::atomic<uint32_t>	_popStartIndex = 0;
		std::atomic<uint32_t>	_popEndIndex = 0;
	};
}

#include "LockFreeQueue.inl"
