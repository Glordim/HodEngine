
namespace hod
{
	/// @brief 
	/// @tparam ...Types 
	template<typename ...Types>
	Event<Types...>::~Event()
	{
		DisconnectAllSlots();
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param ...args 
	template<typename ...Types>
	void Event<Types...>::Emit(Types... args)
	{
		for (Slot* slot : _slots)
		{
			slot->CallFunction(args...);
		}
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param slot 
	template<typename ...Types>
	void Event<Types...>::Connect(Slot& slot)
	{
		_slots.push_back(&slot);
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param slot 
	template<typename ...Types>
	void Event<Types...>::Disconnect(Slot& slot)
	{
		size_t slotCount = _slots.size();
		for (int index = 0; index < slotCount; ++index)
		{
			Slot* currentSlot = _slots[index];
			if (currentSlot == &slot)
			{
				currentSlot->_event = nullptr;

				std::iter_swap(_slots.begin() + index, _slots.end() - 1);
				_slots.pop_back();
				return;
			}
		}
	}

	/// @brief 
	/// @tparam ...Types 
	template<typename ...Types>
	void Event<Types...>::DisconnectAllSlots()
	{
		for (Slot* slot : _slots)
		{
			slot->_event = nullptr;
		}
		_slots.clear();
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param ...args 
	template<typename ...Types>
	inline void Event<Types...>::operator()(Types... args)
	{
		Emit(args...);
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param slot 
	template<typename ...Types>
	inline void Event<Types...>::operator+=(Slot& slot)
	{
		Connect(slot);
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param slot 
	template<typename ...Types>
	inline void Event<Types...>::operator-=(Slot& slot)
	{
		Disconnect(slot);
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param function 
	template<typename ...Types>
	Event<Types...>::Slot::Slot(std::function<void(Types...)> function)
	{
		_function = function;
	}

	/// @brief 
	/// @tparam ...Types 
	template<typename ...Types>
	Event<Types...>::Slot::~Slot()
	{
		Disconnect();
	}

	/// @brief 
	/// @tparam ...Types 
	template<typename ...Types>
	void Event<Types...>::Slot::Disconnect()
	{
		if (_event != nullptr)
		{
			_event->Disconnect(*this);
		}
	}

	/// @brief 
	/// @tparam ...Types 
	/// @return 
	template<typename ...Types>
	bool Event<Types...>::Slot::IsConnected() const
	{
		return (_event != nullptr);
	}

	/// @brief 
	/// @tparam ...Types 
	/// @param ...args 
	template<typename ...Types>
	void Event<Types...>::Slot::CallFunction(Types... args)
	{
		_function(args...);
	}
}
