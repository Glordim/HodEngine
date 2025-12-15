#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Key.hpp"
#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

#include <cstdint>

namespace hod::window
{
	enum class EventType : uint8_t
	{
		KeyPressed,
		KeyReleased,
		Char,
		MouseButtonDown,
		MouseButtonUp,
		MouseMoved,
		MouseScroll,
		MouseHorizontalScroll,
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
		Key      key;
		uint8_t  flags;
	};

	struct CharEventData
	{
		char c;
	};

	struct MouseButtonEventData
	{
		uint16_t    x;
		uint16_t    y;
		MouseButton button;
	};

	struct MouseMoveEventData
	{
		uint16_t x;
		uint16_t y;
	};

	struct MouseScrollEventData
	{
		float value;
	};

	struct Event
	{
		EventType type;

		union
		{
			ResizeEventData      resize;
			KeyEventData         key;
			CharEventData        character;
			MouseButtonEventData mouseButton;
			MouseMoveEventData   mouseMove;
			MouseScrollEventData mouseScroll;
		} data;
	};
}
