#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include "HodEngine/Window/Event.hpp"

#include <mutex>
#include <stdint.h>

namespace hod::window
{
	class Surface;

	/// @brief
	class HOD_WINDOW_API Window
	{
	public:
		Window();
		virtual ~Window() = default;

	public:
		virtual void       Update();
		[[nodiscard]] bool PollEvent(uint32_t& index, Event& event) const;
		[[nodiscard]] bool PollEvent(uint32_t& index, Event& event, uint32_t filterMask) const;

		uint16_t GetWidth() const;
		uint16_t GetHeight() const;

		bool IsClose() const;

	protected:
		void EnqueueEvent(const Event& event);

		void ResizeInternal(uint16_t width, uint16_t height);
		void CloseInternal();

	protected:
		uint16_t _width = 800;
		uint16_t _height = 600;
		bool     _close = false;

		Vector<Event> _events;

		std::mutex    _pendingEventsMutex;
		Vector<Event> _pendingEvents;
	};
}
