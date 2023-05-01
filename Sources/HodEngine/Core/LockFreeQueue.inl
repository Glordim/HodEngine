
namespace hod
{
	/// @brief 
	/// @tparam Type 
	/// @param data 
	/// @return 
	template<typename Type, uint32_t Capacity>
	bool LockFreeQueue<Type, Capacity>::Push(const Type& value)
	{
		while (true)
		{
			uint32_t expectedPushStartIndex = _pushStartIndex.load();
			uint32_t desiredPushStartIndex = (expectedPushStartIndex + 1) % Capacity;

			if (desiredPushStartIndex == _popEndIndex.load()) // Full
			{
				return false;
			}

			if (_pushStartIndex.compare_exchange_weak(expectedPushStartIndex, desiredPushStartIndex) == true)
			{
				new (&_values[expectedPushStartIndex])Type(value);

				while (_pushEndIndex.compare_exchange_weak(expectedPushStartIndex, desiredPushStartIndex) == false);

				return true;
			}
		}
	}

	/// @brief 
	/// @tparam Type 
	/// @param data 
	/// @return 
	template<typename Type, uint32_t Capacity>
	bool LockFreeQueue<Type, Capacity>::Pop(Type& value)
	{
		while (true)
		{
			uint32_t expectedPopStartIndex = _popStartIndex.load();
			uint32_t desiredPopStartIndex = (expectedPopStartIndex + 1) % Capacity;

			if (expectedPopStartIndex == _pushEndIndex.load()) // Empty
			{
				return false;
			}

			if (_popStartIndex.compare_exchange_weak(expectedPopStartIndex, desiredPopStartIndex) == true)
			{
				value = std::move(_values[expectedPopStartIndex]);
				(&_values[expectedPopStartIndex])->~Type();

				while (_popEndIndex.compare_exchange_weak(expectedPopStartIndex, desiredPopStartIndex) != false);

				return true;
			}
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

		return (pushEndIndex >= popStartIndex) ? pushEndIndex - popStartIndex : Capacity - popStartIndex + pushEndIndex;
	}
}
