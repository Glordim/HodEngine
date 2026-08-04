#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/ScanCode.hpp"

namespace hod::inline window
{
	// Mapping Apple keyCode → ScanCode enum (USB HID)
	// Source: Carbon HIToolbox/Events.h kVK_* constants
	ScanCode AppleToHID[128] = {
		// 0x00 – 0x0F
		ScanCode::A,               // 00
		ScanCode::S,               // 01
		ScanCode::D,               // 02
		ScanCode::F,               // 03
		ScanCode::H,               // 04
		ScanCode::G,               // 05
		ScanCode::Z,               // 06
		ScanCode::X,               // 07
		ScanCode::C,               // 08
		ScanCode::V,               // 09
		ScanCode::None,            // 0A (ISO_Section §)
		ScanCode::B,               // 0B
		ScanCode::Q,               // 0C
		ScanCode::W,               // 0D
		ScanCode::E,               // 0E
		ScanCode::R,               // 0F

		// 0x10 – 0x1F
		ScanCode::Y,               // 10
		ScanCode::T,               // 11
		ScanCode::Num1,            // 12
		ScanCode::Num2,            // 13
		ScanCode::Num3,            // 14
		ScanCode::Num4,            // 15
		ScanCode::Num6,            // 16
		ScanCode::Num5,            // 17
		ScanCode::Equal,           // 18
		ScanCode::Num9,            // 19
		ScanCode::Num7,            // 1A
		ScanCode::Minus,           // 1B
		ScanCode::Num8,            // 1C
		ScanCode::Num0,            // 1D
		ScanCode::RightBracket,    // 1E
		ScanCode::O,               // 1F

		// 0x20 – 0x2F
		ScanCode::U,               // 20
		ScanCode::LeftBracket,     // 21
		ScanCode::I,               // 22
		ScanCode::P,               // 23
		ScanCode::Enter,           // 24 (Return)
		ScanCode::L,               // 25
		ScanCode::J,               // 26
		ScanCode::Apostrophe,      // 27
		ScanCode::K,               // 28
		ScanCode::Semicolon,       // 29
		ScanCode::Backslash,       // 2A
		ScanCode::Comma,           // 2B
		ScanCode::Slash,           // 2C
		ScanCode::N,               // 2D
		ScanCode::M,               // 2E
		ScanCode::Period,          // 2F

		// 0x30 – 0x3F
		ScanCode::Tab,             // 30
		ScanCode::Space,           // 31
		ScanCode::Grave,           // 32
		ScanCode::Backspace,       // 33 (Delete/backward delete)
		ScanCode::None,            // 34
		ScanCode::Escape,          // 35
		ScanCode::RightCommand,    // 36
		ScanCode::LeftCommand,     // 37
		ScanCode::LeftShift,       // 38
		ScanCode::CapsLock,        // 39
		ScanCode::LeftAlt,         // 3A (Option)
		ScanCode::LeftCtrl,        // 3B
		ScanCode::RightShift,      // 3C
		ScanCode::RightAlt,        // 3D
		ScanCode::RightCtrl,       // 3E
		ScanCode::None,            // 3F (Function/Fn, no HID equivalent)

		// 0x40 – 0x4F
		ScanCode::None,            // 40 (F17, no ScanCode slot)
		ScanCode::NumPadPeriod,    // 41 (KeypadDecimal)
		ScanCode::None,            // 42
		ScanCode::NumPadMultiply,  // 43 (KeypadMultiply)
		ScanCode::None,            // 44
		ScanCode::NumPadPlus,      // 45 (KeypadPlus)
		ScanCode::None,            // 46
		ScanCode::NumLock,         // 47 (KeypadClear)
		ScanCode::None,            // 48 (VolumeUp, no ScanCode slot)
		ScanCode::None,            // 49 (VolumeDown, no ScanCode slot)
		ScanCode::None,            // 4A (Mute, no ScanCode slot)
		ScanCode::NumPadDivide,    // 4B (KeypadDivide)
		ScanCode::NumPadEnter,     // 4C (KeypadEnter)
		ScanCode::None,            // 4D
		ScanCode::NumPadMinus,     // 4E (KeypadMinus)
		ScanCode::None,            // 4F (F18, no ScanCode slot)

		// 0x50 – 0x5F
		ScanCode::None,            // 50 (F19, no ScanCode slot)
		ScanCode::None,            // 51 (KeypadEquals, no ScanCode slot)
		ScanCode::NumPad0,         // 52
		ScanCode::NumPad1,         // 53
		ScanCode::NumPad2,         // 54
		ScanCode::NumPad3,         // 55
		ScanCode::NumPad4,         // 56
		ScanCode::NumPad5,         // 57
		ScanCode::NumPad6,         // 58
		ScanCode::NumPad7,         // 59
		ScanCode::None,            // 5A (F20, no ScanCode slot)
		ScanCode::NumPad8,         // 5B
		ScanCode::NumPad9,         // 5C
		ScanCode::None,            // 5D (JIS Yen)
		ScanCode::None,            // 5E (JIS Underscore)
		ScanCode::None,            // 5F (JIS KeypadComma)

		// 0x60 – 0x6F
		ScanCode::F5,              // 60
		ScanCode::F6,              // 61
		ScanCode::F7,              // 62
		ScanCode::F3,              // 63
		ScanCode::F8,              // 64
		ScanCode::F9,              // 65
		ScanCode::None,            // 66 (JIS Eisu)
		ScanCode::F11,             // 67
		ScanCode::None,            // 68 (JIS Kana)
		ScanCode::None,            // 69 (F13, no ScanCode slot)
		ScanCode::None,            // 6A (F16, no ScanCode slot)
		ScanCode::None,            // 6B (F14, no ScanCode slot)
		ScanCode::None,            // 6C
		ScanCode::F10,             // 6D
		ScanCode::None,            // 6E
		ScanCode::F12,             // 6F

		// 0x70 – 0x7F
		ScanCode::None,            // 70
		ScanCode::None,            // 71 (F15, no ScanCode slot)
		ScanCode::Insert,          // 72 (Help)
		ScanCode::Home,            // 73
		ScanCode::PageUp,          // 74
		ScanCode::Delete,          // 75 (Forward delete)
		ScanCode::F4,              // 76
		ScanCode::End,             // 77
		ScanCode::F2,              // 78
		ScanCode::PageDown,        // 79
		ScanCode::F1,              // 7A
		ScanCode::Left,            // 7B
		ScanCode::Right,           // 7C
		ScanCode::Down,            // 7D
		ScanCode::Up,              // 7E
		ScanCode::None,            // 7F
	};

	ScanCode MacOSKeyCodeToScanCode(uint16_t keyCode)
	{
		if (keyCode < 128)
			return AppleToHID[keyCode];
		return ScanCode::None;
	}
}
