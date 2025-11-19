#pragma once
#include "HodEngine/Window/Export.hpp"

#include <cstdint>

namespace hod::window
{
	/// @brief CrossPlatform ScanCode based on HID USB Keyboard
	enum class ScanCode : uint8_t
	{
		None = 0x00,

		// Letters
		A = 0x04,
		B = 0x05,
		C = 0x06,
		D = 0x07,
		E = 0x08,
		F = 0x09,
		G = 0x0A,
		H = 0x0B,
		I = 0x0C,
		J = 0x0D,
		K = 0x0E,
		L = 0x0F,
		M = 0x10,
		N = 0x11,
		O = 0x12,
		P = 0x13,
		Q = 0x14,
		R = 0x15,
		S = 0x16,
		T = 0x17,
		U = 0x18,
		V = 0x19,
		W = 0x1A,
		X = 0x1B,
		Y = 0x1C,
		Z = 0x1D,

		// Numbers
		Num1 = 0x1E,
		Num2 = 0x1F,
		Num3 = 0x20,
		Num4 = 0x21,
		Num5 = 0x22,
		Num6 = 0x23,
		Num7 = 0x24,
		Num8 = 0x25,
		Num9 = 0x26,
		Num0 = 0x27,

		// Specials
		Enter = 0x28,
		Escape = 0x29,
		Backspace = 0x2A,
		Tab = 0x2B,
		Space = 0x2C,

		// Symbols
		Minus = 0x2D,        // -
		Equal = 0x2E,        // =
		LeftBracket = 0x2F,  // [
		RightBracket = 0x30, // ]
		Backslash = 0x31,    // \ (US)
		NonUSHash = 0x32,    // # (non-US)
		Semicolon = 0x33,    // ;
		Apostrophe = 0x34,   // '
		Grave = 0x35,        // `
		Comma = 0x36,        // ,
		Period = 0x37,       // .
		Slash = 0x38,        // /

		CapsLock = 0x39,

		// Functions
		F1 = 0x3A,
		F2 = 0x3B,
		F3 = 0x3C,
		F4 = 0x3D,
		F5 = 0x3E,
		F6 = 0x3F,
		F7 = 0x40,
		F8 = 0x41,
		F9 = 0x42,
		F10 = 0x43,
		F11 = 0x44,
		F12 = 0x45,

		// Systems
		PrintScreen = 0x46,
		ScrollLock = 0x47,
		Pause = 0x48,
		Insert = 0x49,
		Home = 0x4A,
		PageUp = 0x4B,
		Delete = 0x4C,
		End = 0x4D,
		PageDown = 0x4E,

		// Arrows
		Right = 0x4F,
		Left = 0x50,
		Down = 0x51,
		Up = 0x52,

		// NumPads
		NumLock = 0x53,
		NumPadDivide = 0x54,   // /
		NumPadMultiply = 0x55, // *
		NumPadMinus = 0x56,    // -
		NumPadPlus = 0x57,     // +
		NumPadEnter = 0x58,
		NumPad1 = 0x59,
		NumPad2 = 0x5A,
		NumPad3 = 0x5B,
		NumPad4 = 0x5C,
		NumPad5 = 0x5D,
		NumPad6 = 0x5E,
		NumPad7 = 0x5F,
		NumPad8 = 0x60,
		NumPad9 = 0x61,
		NumPad0 = 0x62,
		NumPadPeriod = 0x63, // .

		// Others
		NonUSBackslash = 0x64, // Touche supplémentaire (ISO)
		Application = 0x65,    // Menu contextuel

		// Asian Additionals
		International1 = 0x87, // Ro (Japonais)
		International2 = 0x88, // Katakana/Hiragana
		International3 = 0x89, // Yen
		International4 = 0x8A, // Henkan
		International5 = 0x8B, // Muhenkan

		// Modifiers (0xE0-0xE7)
		LeftCtrl = 0xE0,
		LeftShift = 0xE1,
		LeftAlt = 0xE2,
		LeftCommand = 0xE3, // Windows
		RightCtrl = 0xE4,
		RightShift = 0xE5,
		RightAlt = 0xE6,
		RightCommand = 0xE7,
	};

#if defined(PLATFORM_WINDOWS)
	HOD_WINDOW_API ScanCode WindowsScanCodeToScanCode(uint16_t scancode);
#elif defined(PLATFORM_MACOS)
	HOD_WINDOW_API ScanCode MacOSKeyCodeToScanCode(uint16_t keyCode);
#endif
}
