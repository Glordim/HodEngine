
namespace hod
{
	/// @brief
	/// @tparam _Type_
	/// @tparam _Capacity_
	template<typename _Type_, uint32_t _Capacity_>
	LockFreeQueue<_Type_, _Capacity_>::LockFreeQueue()
	: _head(0)
	, _tail(0)
	{
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Capacity_
	/// @param value
	/// @return
	template<typename _Type_, uint32_t _Capacity_>
	bool LockFreeQueue<_Type_, _Capacity_>::Enqueue(const _Type_& value)
	{
		uint32_t currentTail = _tail.load(std::memory_order_relaxed);
		uint32_t nextTail = (currentTail + 1) % _Capacity_;

		if (nextTail == _head.load(std::memory_order_acquire)) // Full
		{
			return false;
		}

		_buffer[currentTail] = value;

		_tail.store(nextTail, std::memory_order_release);
		return true;
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Capacity_
	/// @param result
	/// @return
	template<typename _Type_, uint32_t _Capacity_>
	bool LockFreeQueue<_Type_, _Capacity_>::Dequeue(_Type_& result)
	{
		uint32_t currentHead = _head.load(std::memory_order_relaxed);

		if (currentHead == _tail.load(std::memory_order_acquire)) // Empty
		{
			return false;
		}

		result = _buffer[currentHead];
		_head.store((currentHead + 1) % _Capacity_, std::memory_order_release);
		return true;
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Capacity_
	/// @return
	template<typename _Type_, uint32_t _Capacity_>
	constexpr uint32_t LockFreeQueue<_Type_, _Capacity_>::GetCapacity() const
	{
		return _Capacity_;
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Capacity_
	/// @return
	template<typename _Type_, uint32_t _Capacity_>
	uint32_t LockFreeQueue<_Type_, _Capacity_>::GetSize() const
	{
		uint32_t currentHead = _head.load(std::memory_order_relaxed);
		uint32_t currentTail = _tail.load(std::memory_order_relaxed);

		if (currentTail >= currentHead)
		{
			return currentTail - currentHead;
		}
		else
		{
			return _Capacity_ - (currentHead - currentTail);
		}
	}
}
