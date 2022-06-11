
namespace HOD
{
	/// @brief 
	/// @tparam Type 
	/// @param data 
	/// @return 
	template<typename Type, uint32_t Capacity>
	bool LockFreeQueue<Type, Capacity>::Push(const Type& value)
	{
		uint32_t pushStartIndex = _pushStartIndex.load();
		uint32_t popEndIndex = _popEndIndex.load();

		if ((pushStartIndex + 1) % Capacity == popEndIndex)
		{
			return false;
		}

		if (_pushStartIndex.compare_exchange_weak(pushStartIndex, (pushStartIndex + 1) % Capacity) == true)
		{
			new (&_values[pushStartIndex])Type(value);

			while (_pushEndIndex.compare_exchange_weak(pushStartIndex, (pushStartIndex + 1) % Capacity) == false)

			return true;
		}
	}

	/// @brief 
	/// @tparam Type 
	/// @param data 
	/// @return 
	template<typename Type, uint32_t Capacity>
	bool LockFreeQueue<Type, Capacity>::Pop(Type& value)
	{
		uint32_t popStartIndex = _popStartIndex.load();
		uint32_t pushEndIndex = _pushEndIndex.load();

		if (popStartIndex == pushEndIndex)
		{
			return false;
		}

		if (_popStartIndex.compare_exchange_weak(popStartIndex, (popStartIndex + 1) % Capacity) == true)
		{
			value = std::move(_values[popStartIndex]);
			(&_values[popStartIndex])->~Type();

			while (_popEndIndex.compare_exchange_weak(popStartIndex, (popStartIndex + 1) % Capacity) != false);

			return true;
		}
	}

	/// @brief 
	/// @tparam Type 
	/// @return 
	template<typename Type, uint32_t Capacity>
	inline uint32_t LockFreeQueue<Type, Capacity>::GetCapacity() const
	{
		return Capacity;
	}

	/// @brief 
	/// @tparam Type 
	/// @return 
	template<typename Type, uint32_t Capacity>
	uint32_t LockFreeQueue<Type, Capacity>::GetSize() const
	{
		uint32_t popStartIndex = _popStartIndex.load();
		uint32_t pushEndIndex = _pushEndIndex.load();

		return (pushEndIndex >= popStartIndex) ? pushEndIndex - popStartIndex : N - popStartIndex + pushEndIndex;
	}
}
