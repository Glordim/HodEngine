#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/ScanCode.hpp"

namespace hod::window
{
	// Mapping Apple keyCode → ScanCode enum (USB HID)
	// Source: NSEvent keyCode table / USB HID Usage tables
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
		ScanCode::None,            // 0A (ANSI_Section §)
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
		ScanCode::Backspace,       // 33 (Delete)
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
		ScanCode::None,            // 3F

		// 0x40 – 0x4F
		ScanCode::None,            // 40
		ScanCode::None,            // 41
		ScanCode::None,            // 42
		ScanCode::None,            // 43
		ScanCode::None,            // 44
		ScanCode::None,            // 45
		ScanCode::None,            // 46
		ScanCode::None,            // 47
		ScanCode::NumPadDivide,    // 48
		ScanCode::NumPadMultiply,  // 49
		ScanCode::NumPadMinus,     // 4A
		ScanCode::NumPadPlus,      // 4B
		ScanCode::NumPadEnter,     // 4C
		ScanCode::NumPad1,         // 4D
		ScanCode::NumPad2,         // 4E
		ScanCode::NumPad3,         // 4F

		// 0x50 – 0x5F
		ScanCode::NumPad4,         // 50
		ScanCode::NumPad5,         // 51
		ScanCode::NumPad6,         // 52
		ScanCode::NumPad7,         // 53
		ScanCode::None,            // 54
		ScanCode::NumPad8,         // 55
		ScanCode::NumPad9,         // 56
		ScanCode::None,            // 57
		ScanCode::F5,              // 58
		ScanCode::F6,              // 59
		ScanCode::F7,              // 5A
		ScanCode::F3,              // 5B
		ScanCode::F8,              // 5C
		ScanCode::F9,              // 5D
		ScanCode::None,            // 5E
		ScanCode::F11,             // 5F

		// 0x60 – 0x6F
		ScanCode::None,            // 60
		ScanCode::None,            // 61
		ScanCode::None,            // 62
		ScanCode::None,            // 63
		ScanCode::None,            // 64
		ScanCode::F10,             // 65
		ScanCode::None,            // 66
		ScanCode::F12,             // 67
		ScanCode::None,            // 68
		ScanCode::None,            // 69
		ScanCode::Insert,          // 6A (Help)
		ScanCode::Home,            // 6B
		ScanCode::PageUp,          // 6C
		ScanCode::Delete,          // 6D (Forward delete)
		ScanCode::F4,              // 6E
		ScanCode::End,             // 6F

		// 0x70 – 0x7F
		ScanCode::F2,              // 70
		ScanCode::PageDown,        // 71
		ScanCode::F1,              // 72
		ScanCode::Left,            // 73
		ScanCode::Right,           // 74
		ScanCode::Down,            // 75
		ScanCode::Up,              // 76
		ScanCode::None,            // 77-7F
		ScanCode::None,
		ScanCode::None,
		ScanCode::None,
		ScanCode::None,
		ScanCode::None,
		ScanCode::None,
		ScanCode::None
	};

	ScanCode MacOSKeyCodeToScanCode(uint16_t keyCode)
	{
		if (keyCode < 128)
			return AppleToHID[keyCode];
		return ScanCode::None;
	}
}
