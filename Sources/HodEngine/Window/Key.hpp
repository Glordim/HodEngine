#pragma once
#include "HodEngine/Window/Export.hpp"

#include <cstdint>

namespace hod::window
{
	/// @brief Logical key, depends on the current keyboard layout.
	///        Used for commands and shortcuts, NOT for text input.
	enum class Key : uint8_t
	{
		None = 0,

		// Letters (logical, layout-dependent)
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		// Digits (top row, logical)
		Digit0,
		Digit1,
		Digit2,
		Digit3,
		Digit4,
		Digit5,
		Digit6,
		Digit7,
		Digit8,
		Digit9,

		// Whitespace & editing
		Enter,
		Escape,
		Backspace,
		Tab,
		Space,

		// Punctuation / symbols (logical meaning)
		Minus,
		Equal,
		LeftBracket,
		RightBracket,
		Backslash,
		Semicolon,
		Apostrophe,
		Grave,
		Comma,
		Period,
		Slash,

		// Lock keys
		CapsLock,
		NumLock,
		ScrollLock,

		// Function keys
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,

		// Navigation
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,

		// Arrows
		ArrowLeft,
		ArrowRight,
		ArrowUp,
		ArrowDown,

		// Numpad (logical, not physical)
		Numpad0,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9,

		NumpadAdd,
		NumpadSubtract,
		NumpadMultiply,
		NumpadDivide,
		NumpadDecimal,
		NumpadEnter,

		// System / UI
		PrintScreen,
		Pause,
		Menu,
	};

	enum class Modifier : uint8_t
	{
		None = 0,
		Shift = 1 << 0,
		Ctrl = 1 << 1,
		Alt = 1 << 2,
		Super = 1 << 3, // Cmd / Win
	};

#if defined(PLATFORM_WINDOWS)
	HOD_WINDOW_API Key WindowsVirtualKeyToKey(uint64_t virtualKey);
#endif
}
