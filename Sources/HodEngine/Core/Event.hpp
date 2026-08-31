#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <functional>
#include <utility>
#include <atomic>

namespace hod::inline core
{
	class HOD_CORE_API Delegate
	{
	public:
		~Delegate();

		uint64_t _id = 0;
	};

	/// @brief
	/// @tparam ...Types
	template<typename... Types>
	class Event
	{
	public:
		/// @brief
		class Slot
		{
			friend Event;

		public:
			Slot(std::function<void(Types...)> function);
			Slot(const Slot&) = delete;
			Slot(Slot&&) = delete;
			~Slot();

			void operator=(const Slot&) = delete;
			void operator=(Slot&&) = delete;

		public:
			void Disconnect();
			bool IsConnected() const;

		private:
			void CallFunction(Types... args);

		private:
			std::function<void(Types...)> _function;
			Event*                        _event = nullptr;
		};

	public:
		Event() = default;
		Event(const Event&) = delete;
		Event(Event&&) = delete;
		~Event();

		void operator=(const Event&) = delete;
		void operator=(Event&&) = delete;

	public:
		void     Emit(Types... args);
		Delegate Add(std::function<void(Types...)> function);
		void     Remove(Delegate& delegate);

		void Connect(Slot& slot);
		void Disconnect(Slot& slot);
		void DisconnectAllSlots();

		void operator()(Types... args);
		void operator+=(Slot& slot);
		void operator-=(Slot& slot);

	private:
		Vector<Slot*>                                              _slots;
		Vector<std::pair<Delegate, std::function<void(Types...)>>> _delegates;
		std::atomic<uint64_t>                                      _nextDelegateId = {1};
	};

	/// @brief
	/// @tparam ...Types
	template<typename... Types>
	Event<Types...>::~Event()
	{
		DisconnectAllSlots();
	}

	/// @brief
	/// @tparam ...Types
	/// @param ...args
	template<typename... Types>
	void Event<Types...>::Emit(Types... args)
	{
		for (Slot* slot : _slots)
		{
			slot->CallFunction(args...);
		}

		for (const std::pair<Delegate, std::function<void(Types...)>>& delegate : _delegates)
		{
			delegate.second(args...);
		}
	}

	template<typename... Types>
	Delegate Event<Types...>::Add(std::function<void(Types...)> function)
	{
		Delegate delegate;
		delegate._id = _nextDelegateId.fetch_add(1);
		_delegates.EmplaceBack(delegate, function); // todo lock ?
		return delegate;
	}

	template<typename... Types>
	void Event<Types...>::Remove(Delegate& delegate)
	{
		// todo lock ?
		for (uint32_t i = 0; i < _delegates.Size(); ++i)
		{
			if (_delegates[i].first._id == delegate._id)
			{
				_delegates.Erase(i);
				delegate._id = 0;
				return;
			}
		}
	}

	/// @brief
	/// @tparam ...Types
	/// @param slot
	template<typename... Types>
	void Event<Types...>::Connect(Slot& slot)
	{
		_slots.push_back(&slot);
	}

	/// @brief
	/// @tparam ...Types
	/// @param slot
	template<typename... Types>
	void Event<Types...>::Disconnect(Slot& slot)
	{
		uint32_t slotCount = _slots.Size();
		for (uint32_t index = 0; index < slotCount; ++index)
		{
			Slot* currentSlot = _slots[index];
			if (currentSlot == &slot)
			{
				currentSlot->_event = nullptr;

				std::swap(_slots[index], _slots.Back());
				_slots.PopBack();
				return;
			}
		}
	}

	/// @brief
	/// @tparam ...Types
	template<typename... Types>
	void Event<Types...>::DisconnectAllSlots()
	{
		for (Slot* slot : _slots)
		{
			slot->_event = nullptr;
		}
		_slots.Clear();
	}

	/// @brief
	/// @tparam ...Types
	/// @param ...args
	template<typename... Types>
	inline void Event<Types...>::operator()(Types... args)
	{
		Emit(args...);
	}

	/// @brief
	/// @tparam ...Types
	/// @param slot
	template<typename... Types>
	inline void Event<Types...>::operator+=(Slot& slot)
	{
		Connect(slot);
	}

	/// @brief
	/// @tparam ...Types
	/// @param slot
	template<typename... Types>
	inline void Event<Types...>::operator-=(Slot& slot)
	{
		Disconnect(slot);
	}

	/// @brief
	/// @tparam ...Types
	/// @param function
	template<typename... Types>
	Event<Types...>::Slot::Slot(std::function<void(Types...)> function)
	{
		_function = function;
	}

	/// @brief
	/// @tparam ...Types
	template<typename... Types>
	Event<Types...>::Slot::~Slot()
	{
		Disconnect();
	}

	/// @brief
	/// @tparam ...Types
	template<typename... Types>
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
	template<typename... Types>
	bool Event<Types...>::Slot::IsConnected() const
	{
		return _event != nullptr;
	}

	/// @brief
	/// @tparam ...Types
	/// @param ...args
	template<typename... Types>
	void Event<Types...>::Slot::CallFunction(Types... args)
	{
		_function(args...);
	}
}
