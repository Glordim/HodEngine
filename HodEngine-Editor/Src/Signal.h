#pragma once

#include <vector>
#include <functional>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename ...Types>
class Signal
{
public:

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class Slot
	{
		friend Signal;

	public:

										Slot(std::function<void(Types...)> function);
										Slot(const Slot&) = delete;
										Slot(Slot&&) = delete;
										~Slot() = default;

		void							operator=(const Slot&) = delete;
		void							operator=(Slot&&) = delete;

	public:

		void							Disconnect();
		bool							IsConnected() const;

	private:

		void							CallFunction(Types... args);

	private:

		std::function<void(Types...)>	_function;
		Signal*							_signal = nullptr;
	};

public:

						Signal() = default;
						Signal(const Signal&) = delete;
						Signal(Signal&&) = delete;
						~Signal();

	void				operator=(const Signal&) = delete;
	void				operator=(Signal&&) = delete;

public:

	void				Emit(Types... args);

	void				Connect(Slot& slot);
	void				Disconnect(Slot& slot);
	void				DisconnectAllSlots();

private:

	std::vector<Slot*>	_slots;
};

#include "Signal.inl"
