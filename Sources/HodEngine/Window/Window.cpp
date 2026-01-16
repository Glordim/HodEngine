#include "HodEngine/Window/Pch.hpp"
#include "Window.hpp"

namespace hod::window
{
	Window::Window()
	{
		_pendingEvents.Reserve(1024);
		_events.Reserve(1024);
	}

	/// @brief
	void Window::Update()
	{
		_pendingEventsMutex.lock();
		_pendingEvents.Swap(_events);
		_pendingEvents.Clear();
		_pendingEventsMutex.unlock();
	}

	bool Window::PollEvent(uint32_t& index, Event& event) const
	{
		if (index < _events.Size())
		{
			event = _events[index++];
			return true;
		}
		return false;
	}

	bool Window::PollEvent(uint32_t& index, Event& event, uint32_t filterMask) const
	{
		while (index < _events.Size())
		{
			if (filterMask & (1 << static_cast<uint8_t>(_events[index].type)))
			{
				event = _events[index++];
				return true;
			}
			++index;
		}
		return false;
	}

	void Window::EnqueueEvent(const Event& event)
	{
		_pendingEventsMutex.lock();
		_pendingEvents.PushBack(event);
		_pendingEventsMutex.unlock();
	}

	void Window::ResizeInternal(uint16_t width, uint16_t height)
	{
		if (width != _width || height != _height)
		{
			_width = width;
			_height = height;

			Event event;
			event.type = EventType::Resized;
			event.data.resize.width = width;
			event.data.resize.height = height;
			EnqueueEvent(event);
		}
	}

	/// @brief
	/// @return
	uint16_t Window::GetWidth() const
	{
		return _width;
	}

	/// @brief
	/// @return
	uint16_t Window::GetHeight() const
	{
		return _height;
	}

	void Window::CloseInternal()
	{
		if (_close == false)
		{
			_close = true;

			Event event;
			event.type = EventType::Closed;
			EnqueueEvent(event);
		}
	}

	/// @brief
	/// @return
	bool Window::IsClose() const
	{
		return _close;
	}
}
