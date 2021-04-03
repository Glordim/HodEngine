
//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
Signal<Types...>::~Signal()
{
	DisconnectAllSlots();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::Emit(Types... args)
{
	for (Slot* slot : _slots)
	{
		slot->CallFunction(args...);
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::Connect(Slot& slot)
{
	_slots.push_back(&slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::Disconnect(Slot& slot)
{
	size_t slotCount = _slots.size();
	for (int index = 0; index < slotCount; ++index)
	{
		Slot* currentSlot = _slots[index];
		if (currentSlot == &slot)
		{
			currentSlot->_signal = nullptr;

			std::iter_swap(_slots.begin() + index, _slots.end() - 1);
			_slots.pop_back();
		}
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::DisconnectAllSlots()
{
	for (Slot* slot : _slots)
	{
		slot->_signal = nullptr;
	}
	_slots.clear();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
Signal<Types...>::Slot::Slot(std::function<void(Types...)> function)
{
	_function = function;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::Slot::Disconnect()
{
	if (_signal != nullptr)
	{
		_signal->Disconnect(*this);
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
bool Signal<Types...>::Slot::IsConnected() const
{
	return (_signal != nullptr);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
void Signal<Types...>::Slot::CallFunction(Types... args)
{
	_function(args...);
}
