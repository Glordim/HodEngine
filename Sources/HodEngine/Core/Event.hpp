#pragma once

#include <vector>
#include <functional>
#include <algorithm>

namespace hod
{
	/// @brief 
	/// @tparam ...Types 
	template<typename ...Types>
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

			void							operator=(const Slot&) = delete;
			void							operator=(Slot&&) = delete;

		public:

			void							Disconnect();
			bool							IsConnected() const;

		private:

			void							CallFunction(Types... args);

		private:

			std::function<void(Types...)>	_function;
			Event*							_event = nullptr;
		};

	public:

							Event() = default;
							Event(const Event&) = delete;
							Event(Event&&) = delete;
							~Event();

		void				operator=(const Event&) = delete;
		void				operator=(Event&&) = delete;

	public:

		void				Emit(Types... args);

		void				Connect(Slot& slot);
		void				Disconnect(Slot& slot);
		void				DisconnectAllSlots();

		void				operator()(Types... args);
		void				operator+=(Slot& slot);
		void				operator-=(Slot& slot);

	private:

		std::vector<Slot*>	_slots;
	};
}

#include "Event.inl"
