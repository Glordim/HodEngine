#pragma once

#include <stdint.h>
#include <atomic>

namespace hod
{
	template<typename _Type_, uint32_t _Capacity_>
	class LockFreeQueue
	{
	public:
						LockFreeQueue();
						LockFreeQueue(const LockFreeQueue&) = delete;
						LockFreeQueue(LockFreeQueue&&) = delete;
						~LockFreeQueue() = default;

		LockFreeQueue&	operator = (const LockFreeQueue&) = delete;
		LockFreeQueue&	operator = (LockFreeQueue&&) = delete;

	public:

		bool				Enqueue(const _Type_& value);
		bool				Dequeue(_Type_& result);

		constexpr uint32_t	GetCapacity() const;
		uint32_t			GetSize() const;

	private:
		alignas(64) _Type_					_buffer[_Capacity_];
		alignas(64) std::atomic<uint32_t>	_head;
		alignas(64) std::atomic<uint32_t>	_tail;		
	};
}

#include "LockFreeQueue.inl"
