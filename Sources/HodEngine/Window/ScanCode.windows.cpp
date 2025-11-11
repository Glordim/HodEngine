#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/ScanCode.hpp"

namespace hod::window
{
	// Conversion table: Windows Scan Code Set-1 → USB HID ScanCode
	// Index = Windows scancode, Value = ScanCode enum
	ScanCode Set1ToHID[256] = {
		// 0x00-0x0F
		ScanCode::None,
		ScanCode::Escape,
		ScanCode::Num1,
		ScanCode::Num2,
		ScanCode::Num3,
		ScanCode::Num4,
		ScanCode::Num5,
		ScanCode::Num6,
		ScanCode::Num7,
		ScanCode::Num8,
		ScanCode::Num9,
		ScanCode::Num0,
		ScanCode::Minus,
		ScanCode::Equal,
		ScanCode::Backspace,
		ScanCode::Tab,

		// 0x10-0x1F
		ScanCode::Q,
		ScanCode::W,
		ScanCode::E,
		ScanCode::R,
		ScanCode::T,
		ScanCode::Y,
		ScanCode::U,
		ScanCode::I,
		ScanCode::O,
		ScanCode::P,
		ScanCode::LeftBracket,
		ScanCode::RightBracket,
		ScanCode::Enter,
		ScanCode::LeftCtrl,
		ScanCode::A,
		ScanCode::S,

		// 0x20-0x2F
		ScanCode::D,
		ScanCode::F,
		ScanCode::G,
		ScanCode::H,
		ScanCode::J,
		ScanCode::K,
		ScanCode::L,
		ScanCode::Semicolon,
		ScanCode::Apostrophe,
		ScanCode::Grave,
		ScanCode::LeftShift,
		ScanCode::Backslash,
		ScanCode::Z,
		ScanCode::X,
		ScanCode::C,
		ScanCode::V,

		// 0x30-0x3F
		ScanCode::B,
		ScanCode::N,
		ScanCode::M,
		ScanCode::Comma,
		ScanCode::Period,
		ScanCode::Slash,
		ScanCode::RightShift,
		ScanCode::NumPadMultiply,
		ScanCode::LeftAlt,
		ScanCode::Space,
		ScanCode::CapsLock,
		ScanCode::F1,
		ScanCode::F2,
		ScanCode::F3,
		ScanCode::F4,
		ScanCode::F5,

		// 0x40-0x4F
		ScanCode::F6,
		ScanCode::F7,
		ScanCode::F8,
		ScanCode::F9,
		ScanCode::F10,
		ScanCode::NumLock,
		ScanCode::ScrollLock,
		ScanCode::NumPad7,
		ScanCode::NumPad8,
		ScanCode::NumPad9,
		ScanCode::NumPadMinus,
		ScanCode::NumPad4,
		ScanCode::NumPad5,
		ScanCode::NumPad6,
		ScanCode::NumPadPlus,
		ScanCode::NumPad1,

		// 0x50-0x5F
		ScanCode::NumPad2,
		ScanCode::NumPad3,
		ScanCode::NumPad0,
		ScanCode::NumPadPeriod,
		ScanCode::None,
		ScanCode::None,
		ScanCode::NonUSBackslash,
		ScanCode::F11,
		ScanCode::F12,
		// Rest initialized to ScanCode::None
	};

	ScanCode WindowsScanCodeToScanCode(uint16_t scancode)
	{
		if (scancode < 0x100)
		{
			return Set1ToHID[scancode];
		}
		if (scancode < 0x200)
		{
			switch (scancode)
			{
				case 0x11C: return ScanCode::NumPadEnter;
				case 0x11D: return ScanCode::RightCtrl;
				case 0x135: return ScanCode::NumPadDivide;
				case 0x138: return ScanCode::RightAlt;
				case 0x147: return ScanCode::Home;
				case 0x148: return ScanCode::Up;
				case 0x149: return ScanCode::PageUp;
				case 0x14B: return ScanCode::Left;
				case 0x14D: return ScanCode::Right;
				case 0x14F: return ScanCode::End;
				case 0x150: return ScanCode::Down;
				case 0x151: return ScanCode::PageDown;
				case 0x152: return ScanCode::Insert;
				case 0x153: return ScanCode::Delete;
				case 0x15B: return ScanCode::LeftCommand;
				case 0x15C: return ScanCode::RightCommand;
				case 0x15D: return ScanCode::Application;
				default: return ScanCode::None;
			}
		}
		return ScanCode::None;
	}
}
