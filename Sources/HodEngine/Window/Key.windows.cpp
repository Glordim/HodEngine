#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Key.hpp"

namespace hod::window
{
	// Index = Windows VK (0x00–0x7B)
	// Value = Logical Key (layout-dependent)
	Key VkToKey[0x7B + 1] = {
		/* 0x00 */ Key::None,
		/* 0x01 VK_LBUTTON */ Key::None,
		/* 0x02 VK_RBUTTON */ Key::None,
		/* 0x03 VK_CANCEL */ Key::None,
		/* 0x04 VK_MBUTTON */ Key::None,
		/* 0x05 VK_XBUTTON1 */ Key::None,
		/* 0x06 VK_XBUTTON2 */ Key::None,
		/* 0x07 */ Key::None,

		/* 0x08 VK_BACK */ Key::Backspace,
		/* 0x09 VK_TAB */ Key::Tab,
		/* 0x0A */ Key::None,
		/* 0x0B */ Key::None,
		/* 0x0C VK_CLEAR */ Key::None,
		/* 0x0D VK_RETURN */ Key::Enter,
		/* 0x0E */ Key::None,
		/* 0x0F */ Key::None,

		/* 0x10 VK_SHIFT */ Key::None,
		/* 0x11 VK_CONTROL */ Key::None,
		/* 0x12 VK_MENU */ Key::None,
		/* 0x13 VK_PAUSE */ Key::Pause,
		/* 0x14 VK_CAPITAL */ Key::CapsLock,
		/* 0x15 VK_KANA */ Key::None,
		/* 0x16 */ Key::None,
		/* 0x17 VK_JUNJA */ Key::None,

		/* 0x18 VK_FINAL */ Key::None,
		/* 0x19 VK_KANJI */ Key::None,
		/* 0x1A */ Key::None,
		/* 0x1B VK_ESCAPE */ Key::Escape,
		/* 0x1C VK_CONVERT */ Key::None,
		/* 0x1D VK_NONCONVERT */ Key::None,
		/* 0x1E VK_ACCEPT */ Key::None,
		/* 0x1F VK_MODECHANGE */ Key::None,

		/* 0x20 VK_SPACE */ Key::Space,
		/* 0x21 VK_PRIOR */ Key::PageUp,
		/* 0x22 VK_NEXT */ Key::PageDown,
		/* 0x23 VK_END */ Key::End,
		/* 0x24 VK_HOME */ Key::Home,
		/* 0x25 VK_LEFT */ Key::ArrowLeft,
		/* 0x26 VK_UP */ Key::ArrowUp,
		/* 0x27 VK_RIGHT */ Key::ArrowRight,

		/* 0x28 VK_DOWN */ Key::ArrowDown,
		/* 0x29 VK_SELECT */ Key::None,
		/* 0x2A VK_PRINT */ Key::None,
		/* 0x2B VK_EXECUTE */ Key::None,
		/* 0x2C VK_SNAPSHOT */ Key::PrintScreen,
		/* 0x2D VK_INSERT */ Key::Insert,
		/* 0x2E VK_DELETE */ Key::Delete,
		/* 0x2F VK_HELP */ Key::None,

		/* 0x30 '0' */ Key::Digit0,
		/* 0x31 '1' */ Key::Digit1,
		/* 0x32 '2' */ Key::Digit2,
		/* 0x33 '3' */ Key::Digit3,
		/* 0x34 '4' */ Key::Digit4,
		/* 0x35 '5' */ Key::Digit5,
		/* 0x36 '6' */ Key::Digit6,
		/* 0x37 '7' */ Key::Digit7,

		/* 0x38 '8' */ Key::Digit8,
		/* 0x39 '9' */ Key::Digit9,
		/* 0x3A */ Key::None,
		/* 0x3B */ Key::None,
		/* 0x3C */ Key::None,
		/* 0x3D */ Key::None,
		/* 0x3E */ Key::None,
		/* 0x3F */ Key::None,

		/* 0x40 */ Key::None,
		/* 0x41 VK_A */ Key::A,
		/* 0x42 VK_B */ Key::B,
		/* 0x43 VK_C */ Key::C,
		/* 0x44 VK_D */ Key::D,
		/* 0x45 VK_E */ Key::E,
		/* 0x46 VK_F */ Key::F,
		/* 0x47 VK_G */ Key::G,

		/* 0x48 VK_H */ Key::H,
		/* 0x49 VK_I */ Key::I,
		/* 0x4A VK_J */ Key::J,
		/* 0x4B VK_K */ Key::K,
		/* 0x4C VK_L */ Key::L,
		/* 0x4D VK_M */ Key::M,
		/* 0x4E VK_N */ Key::N,
		/* 0x4F VK_O */ Key::O,

		/* 0x50 VK_P */ Key::P,
		/* 0x51 VK_Q */ Key::Q,
		/* 0x52 VK_R */ Key::R,
		/* 0x53 VK_S */ Key::S,
		/* 0x54 VK_T */ Key::T,
		/* 0x55 VK_U */ Key::U,
		/* 0x56 VK_V */ Key::V,
		/* 0x57 VK_W */ Key::W,

		/* 0x58 VK_X */ Key::X,
		/* 0x59 VK_Y */ Key::Y,
		/* 0x5A VK_Z */ Key::Z,
		/* 0x5B VK_LWIN */ Key::None,
		/* 0x5C VK_RWIN */ Key::None,
		/* 0x5D VK_APPS */ Key::Menu,
		/* 0x5E */ Key::None,
		/* 0x5F VK_SLEEP */ Key::None,

		/* 0x60 VK_NUMPAD0 */ Key::Numpad0,
		/* 0x61 VK_NUMPAD1 */ Key::Numpad1,
		/* 0x62 VK_NUMPAD2 */ Key::Numpad2,
		/* 0x63 VK_NUMPAD3 */ Key::Numpad3,
		/* 0x64 VK_NUMPAD4 */ Key::Numpad4,
		/* 0x65 VK_NUMPAD5 */ Key::Numpad5,
		/* 0x66 VK_NUMPAD6 */ Key::Numpad6,
		/* 0x67 VK_NUMPAD7 */ Key::Numpad7,

		/* 0x68 VK_NUMPAD8 */ Key::Numpad8,
		/* 0x69 VK_NUMPAD9 */ Key::Numpad9,
		/* 0x6A VK_MULTIPLY */ Key::NumpadMultiply,
		/* 0x6B VK_ADD */ Key::NumpadAdd,
		/* 0x6C VK_SEPARATOR */ Key::None,
		/* 0x6D VK_SUBTRACT */ Key::NumpadSubtract,
		/* 0x6E VK_DECIMAL */ Key::NumpadDecimal,
		/* 0x6F VK_DIVIDE */ Key::NumpadDivide,

		/* 0x70 VK_F1 */ Key::F1,
		/* 0x71 VK_F2 */ Key::F2,
		/* 0x72 VK_F3 */ Key::F3,
		/* 0x73 VK_F4 */ Key::F4,
		/* 0x74 VK_F5 */ Key::F5,
		/* 0x75 VK_F6 */ Key::F6,
		/* 0x76 VK_F7 */ Key::F7,
		/* 0x77 VK_F8 */ Key::F8,

		/* 0x78 VK_F9 */ Key::F9,
		/* 0x79 VK_F10 */ Key::F10,
		/* 0x7A VK_F11 */ Key::F11,
		/* 0x7B VK_F12 */ Key::F12,
	};

	Key WindowsVirtualKeyToKey(uint64_t virtualKey)
	{
		return (virtualKey <= 0x7B) ? VkToKey[virtualKey] : Key::None;
	}
}
