#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Event.hpp"

namespace hod::inline core
{
	/// @brief Its connections disconnected. Destroyed by a callback while it emits: the running Emits stop, its entries deleted by the outermost
	/// once it ends (one of them is running)
	EventBase::~EventBase()
	{
		for (EventEntry* entry : _entries)
		{
			if (entry->_connection != nullptr)
			{
				entry->_connection->_entry = nullptr;
				entry->_connection = nullptr;
			}
			entry->_event = nullptr;
		}

		if (_emitScope != nullptr)
		{
			EmitScope* outermost = _emitScope;
			for (EmitScope* scope = _emitScope; scope != nullptr; scope = scope->_previous)
			{
				scope->_destroyed = true;
				outermost = scope;
			}
			for (EventEntry* entry : _entries)
			{
				outermost->_orphans.PushBack(entry);
			}
		}
		else
		{
			for (EventEntry* entry : _entries)
			{
				delete entry;
			}
		}
	}

	/// @brief
	void EventBase::DisconnectAll()
	{
		for (EventEntry* entry : _entries)
		{
			if (entry->_connection != nullptr)
			{
				entry->_connection->_entry = nullptr;
				entry->_connection = nullptr;
			}
		}

		if (_emitScope != nullptr)
		{
			_hasRemovedEntries = true; // Deleted once the outermost Emit ends
		}
		else
		{
			Vector<EventEntry*> entries = std::move(_entries); // Deleting a function may destroy the event (its captures): nothing touched after
			_entries.Clear();
			for (EventEntry* entry : entries)
			{
				delete entry;
			}
		}
	}

	/// @brief
	/// @return
	uint32_t EventBase::GetConnectionCount() const
	{
		uint32_t count = 0;
		for (const EventEntry* entry : _entries)
		{
			if (entry->_connection != nullptr)
			{
				++count;
			}
		}
		return count;
	}

	/// @brief
	/// @param entry
	/// @return
	EventConnection EventBase::Add(EventEntry* entry)
	{
		entry->_event = this;
		_entries.PushBack(entry);
		return EventConnection(entry);
	}

	/// @brief
	/// @param entry
	void EventBase::Remove(EventEntry* entry)
	{
		entry->_connection = nullptr;

		if (_emitScope != nullptr)
		{
			_hasRemovedEntries = true; // Not erased while it emits (its loop indexes them), deleted once the outermost Emit ends
			return;
		}

		for (uint32_t index = 0; index < _entries.Size(); ++index)
		{
			if (_entries[index] == entry)
			{
				_entries.Erase(index); // Not swapped with the last: the connection order is the call order
				break;
			}
		}
		delete entry; // Last: its function's captures may destroy the event
	}

	/// @brief
	/// @param scope
	void EventBase::BeginEmit(EmitScope& scope)
	{
		scope._previous = _emitScope;
		_emitScope = &scope;
	}

	/// @brief The entries disconnected while it emitted deleted once the outermost ends
	/// @param scope
	void EventBase::EndEmit(EmitScope& scope)
	{
		_emitScope = scope._previous;
		if (_emitScope != nullptr || _hasRemovedEntries == false)
		{
			return;
		}
		_hasRemovedEntries = false;

		Vector<EventEntry*> removedEntries;
		uint32_t            keptCount = 0;
		for (uint32_t index = 0; index < _entries.Size(); ++index)
		{
			EventEntry* entry = _entries[index];
			if (entry->_connection != nullptr)
			{
				_entries[keptCount] = entry;
				++keptCount;
			}
			else
			{
				removedEntries.PushBack(entry);
			}
		}
		_entries.Resize(keptCount);

		// Last: their functions' captures may destroy the event
		for (EventEntry* entry : removedEntries)
		{
			delete entry;
		}
	}

	/// @brief
	/// @param scope
	void EventBase::EndDestroyedEmit(EmitScope& scope)
	{
		if (scope._previous == nullptr)
		{
			for (EventEntry* entry : scope._orphans)
			{
				delete entry;
			}
		}
	}

	/// @brief
	/// @param entry
	/// @return
	bool EventBase::IsConnected(const EventEntry* entry)
	{
		return entry->_connection != nullptr;
	}

	/// @brief
	/// @param entry
	EventConnection::EventConnection(EventEntry* entry)
	: _entry(entry)
	{
		_entry->_connection = this;
	}

	/// @brief
	/// @param other
	EventConnection::EventConnection(EventConnection&& other)
	: _entry(other._entry)
	{
		other._entry = nullptr;
		if (_entry != nullptr)
		{
			_entry->_connection = this;
		}
	}

	/// @brief
	EventConnection::~EventConnection()
	{
		Disconnect();
	}

	/// @brief Its previous connection disconnected
	/// @param other
	/// @return
	EventConnection& EventConnection::operator=(EventConnection&& other)
	{
		if (this != &other)
		{
			Disconnect();
			_entry = other._entry;
			other._entry = nullptr;
			if (_entry != nullptr)
			{
				_entry->_connection = this;
			}
		}
		return *this;
	}

	/// @brief
	void EventConnection::Disconnect()
	{
		if (_entry != nullptr)
		{
			EventEntry* entry = _entry;
			_entry = nullptr;
			entry->_event->Remove(entry);
		}
	}

	/// @brief
	/// @return
	bool EventConnection::IsConnected() const
	{
		return _entry != nullptr;
	}

	/// @brief
	/// @param event
	/// @return
	bool EventConnection::IsConnectedTo(const EventBase& event) const
	{
		return _entry != nullptr && _entry->_event == &event;
	}

	/// @brief The connections of the events destroyed meanwhile forgotten
	/// @param connection
	void EventListener::Add(EventConnection&& connection)
	{
		for (uint32_t index = _connections.Size(); index > 0; --index)
		{
			if (_connections[index - 1].IsConnected() == false)
			{
				_connections.Erase(index - 1);
			}
		}
		_connections.PushBack(std::move(connection));
	}

	/// @brief
	/// @param event
	void EventListener::StopListening(const EventBase& event)
	{
		for (uint32_t index = _connections.Size(); index > 0; --index)
		{
			if (_connections[index - 1].IsConnectedTo(event))
			{
				_connections[index - 1].Disconnect();
				_connections.Erase(index - 1);
			}
		}
	}

	/// @brief
	void EventListener::StopListeningAll()
	{
		Vector<EventConnection> connections = std::move(_connections);
		_connections.Clear();
		// Disconnected by their destruction
	}

	/// @brief
	/// @param event
	/// @return
	bool EventListener::IsListening(const EventBase& event) const
	{
		for (const EventConnection& connection : _connections)
		{
			if (connection.IsConnectedTo(event))
			{
				return true;
			}
		}
		return false;
	}
}
