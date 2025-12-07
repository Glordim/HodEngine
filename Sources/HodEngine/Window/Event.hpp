#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

#include <cstdint>

namespace hod::window
{
	enum class EventType : uint8_t
	{
		KeyPressed,
		KeyReleased,
		MouseButtonDown,
		MouseButtonUp,
		MouseMoved,
		Closed,
		Resized,
		FocusGained,
		FocusLost,
	};

	struct ResizeEventData
	{
		uint16_t width;
		uint16_t height;
	};

	struct KeyEventData
	{
		enum class Flag : uint8_t
		{
			Repeat,
			Shift,
			Ctrl,
			Alt,
			Meta,
		};

		ScanCode scanCode;
		uint8_t  flags;
	};

	struct MouseButtonEventData
	{
		MouseButton button;
		uint16_t    x;
		uint16_t    y;
	};

	struct MouseMoveEventData
	{
		uint16_t x;
		uint16_t y;
	};

	struct MouseScrollEventData
	{
		uint16_t x;
		uint16_t y;
	};

	struct Event
	{
		EventType type;

		union
		{
			ResizeEventData      resize;
			KeyEventData         key;
			MouseButtonEventData mouseButton;
			MouseMoveEventData   mouseMove;
			MouseScrollEventData mouseScroll;
		} data;
	};
}
