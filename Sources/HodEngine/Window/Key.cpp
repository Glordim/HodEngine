#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Key.hpp"

namespace hod::inline window
{
	Key ScanCodeToKey(ScanCode scanCode)
	{
		switch (scanCode)
		{
			case ScanCode::A: return Key::A;
			case ScanCode::B: return Key::B;
			case ScanCode::C: return Key::C;
			case ScanCode::D: return Key::D;
			case ScanCode::E: return Key::E;
			case ScanCode::F: return Key::F;
			case ScanCode::G: return Key::G;
			case ScanCode::H: return Key::H;
			case ScanCode::I: return Key::I;
			case ScanCode::J: return Key::J;
			case ScanCode::K: return Key::K;
			case ScanCode::L: return Key::L;
			case ScanCode::M: return Key::M;
			case ScanCode::N: return Key::N;
			case ScanCode::O: return Key::O;
			case ScanCode::P: return Key::P;
			case ScanCode::Q: return Key::Q;
			case ScanCode::R: return Key::R;
			case ScanCode::S: return Key::S;
			case ScanCode::T: return Key::T;
			case ScanCode::U: return Key::U;
			case ScanCode::V: return Key::V;
			case ScanCode::W: return Key::W;
			case ScanCode::X: return Key::X;
			case ScanCode::Y: return Key::Y;
			case ScanCode::Z: return Key::Z;

			case ScanCode::Num1: return Key::Digit1;
			case ScanCode::Num2: return Key::Digit2;
			case ScanCode::Num3: return Key::Digit3;
			case ScanCode::Num4: return Key::Digit4;
			case ScanCode::Num5: return Key::Digit5;
			case ScanCode::Num6: return Key::Digit6;
			case ScanCode::Num7: return Key::Digit7;
			case ScanCode::Num8: return Key::Digit8;
			case ScanCode::Num9: return Key::Digit9;
			case ScanCode::Num0: return Key::Digit0;

			case ScanCode::Enter: return Key::Enter;
			case ScanCode::Escape: return Key::Escape;
			case ScanCode::Backspace: return Key::Backspace;
			case ScanCode::Tab: return Key::Tab;
			case ScanCode::Space: return Key::Space;

			case ScanCode::Minus: return Key::Minus;
			case ScanCode::Equal: return Key::Equal;
			case ScanCode::LeftBracket: return Key::LeftBracket;
			case ScanCode::RightBracket: return Key::RightBracket;
			case ScanCode::Backslash: return Key::Backslash;
			case ScanCode::Semicolon: return Key::Semicolon;
			case ScanCode::Apostrophe: return Key::Apostrophe;
			case ScanCode::Grave: return Key::Grave;
			case ScanCode::Comma: return Key::Comma;
			case ScanCode::Period: return Key::Period;
			case ScanCode::Slash: return Key::Slash;

			case ScanCode::CapsLock: return Key::CapsLock;

			case ScanCode::F1: return Key::F1;
			case ScanCode::F2: return Key::F2;
			case ScanCode::F3: return Key::F3;
			case ScanCode::F4: return Key::F4;
			case ScanCode::F5: return Key::F5;
			case ScanCode::F6: return Key::F6;
			case ScanCode::F7: return Key::F7;
			case ScanCode::F8: return Key::F8;
			case ScanCode::F9: return Key::F9;
			case ScanCode::F10: return Key::F10;
			case ScanCode::F11: return Key::F11;
			case ScanCode::F12: return Key::F12;

			case ScanCode::PrintScreen: return Key::PrintScreen;
			case ScanCode::ScrollLock: return Key::ScrollLock;
			case ScanCode::Pause: return Key::Pause;
			case ScanCode::Insert: return Key::Insert;
			case ScanCode::Home: return Key::Home;
			case ScanCode::PageUp: return Key::PageUp;
			case ScanCode::Delete: return Key::Delete;
			case ScanCode::End: return Key::End;
			case ScanCode::PageDown: return Key::PageDown;

			case ScanCode::Right: return Key::ArrowRight;
			case ScanCode::Left: return Key::ArrowLeft;
			case ScanCode::Down: return Key::ArrowDown;
			case ScanCode::Up: return Key::ArrowUp;

			case ScanCode::NumLock: return Key::NumLock;
			case ScanCode::NumPadDivide: return Key::NumpadDivide;
			case ScanCode::NumPadMultiply: return Key::NumpadMultiply;
			case ScanCode::NumPadMinus: return Key::NumpadSubtract;
			case ScanCode::NumPadPlus: return Key::NumpadAdd;
			case ScanCode::NumPadEnter: return Key::NumpadEnter;
			case ScanCode::NumPad1: return Key::Numpad1;
			case ScanCode::NumPad2: return Key::Numpad2;
			case ScanCode::NumPad3: return Key::Numpad3;
			case ScanCode::NumPad4: return Key::Numpad4;
			case ScanCode::NumPad5: return Key::Numpad5;
			case ScanCode::NumPad6: return Key::Numpad6;
			case ScanCode::NumPad7: return Key::Numpad7;
			case ScanCode::NumPad8: return Key::Numpad8;
			case ScanCode::NumPad9: return Key::Numpad9;
			case ScanCode::NumPad0: return Key::Numpad0;
			case ScanCode::NumPadPeriod: return Key::NumpadDecimal;

			case ScanCode::Application: return Key::Menu;

			case ScanCode::LeftCtrl:
			case ScanCode::RightCtrl: return Key::Ctrl;
			case ScanCode::LeftShift:
			case ScanCode::RightShift: return Key::Shift;
			case ScanCode::LeftAlt:
			case ScanCode::RightAlt: return Key::Alt;
			case ScanCode::LeftCommand:
			case ScanCode::RightCommand: return Key::Super;

			default: return Key::None;
		}
	}
}
