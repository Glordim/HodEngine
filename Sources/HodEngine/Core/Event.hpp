#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <functional>
#include <utility>

namespace hod::inline core
{
	class EventBase;
	class EventConnection;
	template<typename... Types>
	class Event;

	/// @brief A function connected to an event: allocated alone, it doesn't move while called (the event's list may grow meanwhile)
	class HOD_CORE_API EventEntry
	{
	public:
		virtual ~EventEntry() = default;

	private:
		friend EventBase;
		friend EventConnection;

		EventBase*       _event = nullptr;
		EventConnection* _connection = nullptr; // Null once disconnected
	};

	/// @brief What Event<Types...> doesn't need its types for: its connections, cut safely while it emits, and its destruction while it emits.
	/// A callback may connect (the new function is called from the next Emit), disconnect any function (itself included: not called anymore,
	/// destroyed once the outermost Emit ends), emit again (reentrance), or destroy the event (the Emit stops at once). Not thread safe
	class HOD_CORE_API EventBase
	{
	public:
		EventBase(const EventBase&) = delete;
		EventBase(EventBase&&) = delete;

		void operator=(const EventBase&) = delete;
		void operator=(EventBase&&) = delete;

	public:
		/// @brief Its connections cut (their EventConnection disconnected)
		void		DisconnectAll();
		/// @return The count of connected functions
		uint32_t	GetConnectionCount() const;

	protected:
		/// @brief One per Emit running, on its stack, chained (reentrance)
		struct EmitScope
		{
			EmitScope*			_previous = nullptr;
			bool				_destroyed = false; // The event destroyed by a callback: nothing of it to touch anymore
			Vector<EventEntry*>	_orphans;           // The outermost's: the entries of the destroyed event, deleted once it ends (one of them may be running)
		};

	protected:
		EventBase() = default;
		~EventBase();

		/// @return Its connection
		EventConnection	Add(EventEntry* entry);

		void	BeginEmit(EmitScope& scope);
		/// @brief Not called when the event was destroyed (scope._destroyed): see EndDestroyedEmit
		void	EndEmit(EmitScope& scope);
		/// @brief The orphaned entries deleted by the outermost
		static void	EndDestroyedEmit(EmitScope& scope);

		/// @return false once disconnected: not called anymore
		static bool	IsConnected(const EventEntry* entry);

	protected:
		Vector<EventEntry*>	_entries; // In the connection order

	private:
		friend EventConnection;

		void	Remove(EventEntry* entry);

	private:
		EmitScope*	_emitScope = nullptr; // The innermost running Emit, null when it doesn't emit
		bool		_hasRemovedEntries = false;
	};

	/// @brief A function connected to an event, disconnected when destroyed: hold it as long as it must be called. Movable, not copyable.
	/// The event may be destroyed first: the connection is then disconnected
	class [[nodiscard]] HOD_CORE_API EventConnection
	{
	public:
		EventConnection() = default;
		EventConnection(const EventConnection&) = delete;
		EventConnection(EventConnection&& other);
		~EventConnection();

		void				operator=(const EventConnection&) = delete;
		EventConnection&	operator=(EventConnection&& other);

	public:
		/// @brief Not called anymore, even from the Emit running
		void	Disconnect();
		bool	IsConnected() const;
		/// @return true when connected to this event
		bool	IsConnectedTo(const EventBase& event) const;

	private:
		friend EventBase;

		explicit EventConnection(EventEntry* entry);

	private:
		EventEntry* _entry = nullptr;
	};

	/// @brief Connections held together, cut when it is destroyed: a member of any class listening to events (a service, an editor window...)
	class HOD_CORE_API EventListener
	{
	public:
		EventListener() = default;
		EventListener(const EventListener&) = delete;
		EventListener(EventListener&&) = default;
		~EventListener() = default;

		void			operator=(const EventListener&) = delete;
		EventListener&	operator=(EventListener&&) = default;

	public:
		/// @brief object->method(args...) on each Emit, until StopListening
		template<typename... Types, typename Object, typename Method>
		void	Listen(Event<Types...>& event, Object* object, Method method);
		/// @brief function(args...) on each Emit, until StopListening
		template<typename... Types, typename Function>
		void	Listen(Event<Types...>& event, Function&& function);

		/// @brief Every function it connected to this event (none called anymore, even from the Emit running)
		void	StopListening(const EventBase& event);
		void	StopListeningAll();
		bool	IsListening(const EventBase& event) const;

	private:
		void	Add(EventConnection&& connection);

	private:
		Vector<EventConnection> _connections;
	};

	/// @brief Functions called in their connection order by Emit (see EventBase for what they may do meanwhile)
	/// @tparam ...Types Its arguments
	template<typename... Types>
	class Event : public EventBase
	{
	public:
		Event() = default;
		~Event() = default;

	public:
		void	Emit(Types... args);

		/// @brief object->method(args...) on each Emit, until its connection is disconnected or destroyed
		template<typename Object, typename Method>
		EventConnection	Connect(Object* object, Method method);
		/// @brief function(args...) on each Emit, until its connection is disconnected or destroyed
		template<typename Function>
		EventConnection	Connect(Function&& function);

	private:
		class Entry : public EventEntry
		{
		public:
			explicit Entry(std::function<void(Types...)> function)
			: _function(std::move(function))
			{
			}

			std::function<void(Types...)> _function;
		};
	};

	/// @brief
	/// @tparam ...Types
	/// @param ...args
	template<typename... Types>
	void Event<Types...>::Emit(Types... args)
	{
		EmitScope scope;
		BeginEmit(scope);

		// Up to the count at its start: a function connected meanwhile is called from the next Emit. No entry is erased while it emits
		uint32_t count = _entries.Size();
		for (uint32_t index = 0; index < count; ++index)
		{
			EventEntry* entry = _entries[index];
			if (IsConnected(entry))
			{
				static_cast<Entry*>(entry)->_function(args...);
				if (scope._destroyed)
				{
					EndDestroyedEmit(scope);
					return;
				}
			}
		}

		EndEmit(scope);
	}

	/// @brief
	/// @tparam ...Types
	/// @tparam Object
	/// @tparam Method
	/// @param object
	/// @param method
	/// @return
	template<typename... Types>
	template<typename Object, typename Method>
	EventConnection Event<Types...>::Connect(Object* object, Method method)
	{
		return Add(new Entry([object, method](Types... args) { (object->*method)(args...); }));
	}

	/// @brief
	/// @tparam ...Types
	/// @tparam Function
	/// @param function
	/// @return
	template<typename... Types>
	template<typename Function>
	EventConnection Event<Types...>::Connect(Function&& function)
	{
		return Add(new Entry(std::function<void(Types...)>(std::forward<Function>(function))));
	}

	/// @brief
	/// @tparam ...Types
	/// @tparam Object
	/// @tparam Method
	/// @param event
	/// @param object
	/// @param method
	template<typename... Types, typename Object, typename Method>
	void EventListener::Listen(Event<Types...>& event, Object* object, Method method)
	{
		Add(event.Connect(object, method));
	}

	/// @brief
	/// @tparam ...Types
	/// @tparam Function
	/// @param event
	/// @param function
	template<typename... Types, typename Function>
	void EventListener::Listen(Event<Types...>& event, Function&& function)
	{
		Add(event.Connect(std::forward<Function>(function)));
	}
}
