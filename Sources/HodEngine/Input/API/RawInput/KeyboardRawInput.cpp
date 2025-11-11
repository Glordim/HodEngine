#include <array>
#include <limits>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/RawInput/KeyboardRawInput.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"

#include <Windows.h>

#undef max

namespace hod::input
{
	/*
	The scancode values come from:
	- http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/scancode.doc (March 16, 2000).
	- http://www.computer-engineering.org/ps2keyboard/scancodes1.html
	- using MapVirtualKeyEx( VK_*, MAPVK_VK_TO_VSC_EX, 0 ) with the english us keyboard layout
	- reading win32 WM_INPUT keyboard messages.
	*/
	enum class WINDOWS_SCAN_CODE : USHORT
	{
		SC_ESCAPE = 0x01,
		SC_1 = 0x02,
		SC_2 = 0x03,
		SC_3 = 0x04,
		SC_4 = 0x05,
		SC_5 = 0x06,
		SC_6 = 0x07,
		SC_7 = 0x08,
		SC_8 = 0x09,
		SC_9 = 0x0A,
		SC_0 = 0x0B,
		SC_SUBSTRACT = 0x0C,
		SC_EQUAL = 0x0D,
		SC_BACKSPACE = 0x0E,
		SC_TAB = 0x0F,
		SC_Q = 0x10,
		SC_W = 0x11,
		SC_E = 0x12,
		SC_R = 0x13,
		SC_T = 0x14,
		SC_Y = 0x15,
		SC_U = 0x16,
		SC_I = 0x17,
		SC_O = 0x18,
		SC_P = 0x19,
		SC_LEFT_BRACKET = 0x1A,
		SC_RIGHT_BRACKET = 0x1B,
		SC_ENTER = 0x1C,
		SC_LEFT_CONTROL = 0x1D,
		SC_A = 0x1E,
		SC_S = 0x1F,
		SC_D = 0x20,
		SC_F = 0x21,
		SC_G = 0x22,
		SC_H = 0x23,
		SC_J = 0x24,
		SC_K = 0x25,
		SC_L = 0x26,
		SC_SEMICOLON = 0x27,
		SC_APOSTROPHE = 0x28,
		SC_TILDE = 0x29,
		SC_LEFT_SHIFT = 0x2A,
		SC_BACKSLASH = 0x2B,
		SC_Z = 0x2C,
		SC_X = 0x2D,
		SC_C = 0x2E,
		SC_V = 0x2F,
		SC_B = 0x30,
		SC_N = 0x31,
		SC_M = 0x32,
		SC_COMMA = 0x33,
		SC_PERIOD = 0x34,
		SC_SLASH = 0x35,
		SC_RIGHT_SHIFT = 0x36,
		SC_NUMPAD_MULTIPLY = 0x37,
		SC_LEFT_ALT = 0x38,
		SC_SPACE = 0x39,
		SC_CAPS_LOCK = 0x3A,
		SC_F1 = 0x3B,
		SC_F2 = 0x3C,
		SC_F3 = 0x3D,
		SC_F4 = 0x3E,
		SC_F5 = 0x3F,
		SC_F6 = 0x40,
		SC_F7 = 0x41,
		SC_F8 = 0x42,
		SC_F9 = 0x43,
		SC_F10 = 0x44,
		SC_NUM_LOCK = 0x45,
		SC_SCROLL_LOCK = 0x46,
		SC_NUMPAD_7 = 0x47,
		SC_NUMPAD_8 = 0x48,
		SC_NUMPAD_9 = 0x49,
		SC_NUMPAD_SUBTRACT = 0x4A,
		SC_NUMPAD_4 = 0x4B,
		SC_NUMPAD_5 = 0x4C,
		SC_NUMPAD_6 = 0x4D,
		SC_NUMPAD_ADD = 0x4E,
		SC_NUMPAD_1 = 0x4F,
		SC_NUMPAD_2 = 0x50,
		SC_NUMPAD_3 = 0x51,
		SC_NUMPAD_0 = 0x52,
		SC_NUMPAD_DECIMAL = 0x53,
		SC_SYS_RQ = 0x54,  /* Alt + print screen. MapVirtualKeyEx( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) returns scancode 0x54. */
		SC_OEM_102 = 0x56, /* <> or \| on RT 102-key keyboard (Non-U.S.) */
		SC_F11 = 0x57,
		SC_F12 = 0x58,
		SC_OEM_1 = 0x5a, /* VK_OEM_WSCTRL */
		SC_OEM_2 = 0x5b, /* VK_OEM_FINISH */
		SC_OEM_3 = 0x5c, /* VK_OEM_JUMP */
		SC_ERASE_EOF = 0x5d,
		SC_OEM_4 = 0x5e, /* VK_OEM_BACKTAB */
		SC_OEM_5 = 0x5f, /* VK_OEM_AUTO */
		SC_ZOOM_FUNC = 0x62,
		SC_HELP = 0x63,
		SC_F13 = 0x64, /*                     (NEC PC98) */
		SC_F14 = 0x65, /*                     (NEC PC98) */
		SC_F15 = 0x66, /*                     (NEC PC98) */
		SC_F16 = 0x67,
		SC_F17 = 0x68,
		SC_F18 = 0x69,
		SC_F19 = 0x6a,
		SC_F20 = 0x6b,
		SC_F21 = 0x6c,
		SC_F22 = 0x6d,
		SC_F23 = 0x6e,
		SC_OEM_6 = 0x6f,    /* VK_OEM_PA3 */
		SC_KATAKANA = 0x70, /* (Japanese keyboard)            */
		SC_OEM_7 = 0x71,    /* VK_OEM_RESET */
		SC_ABNT_C1 = 0x73,  /* /? on Brazilian keyboard */
		SC_F24 = 0x76,
		SC_SBCSCHAR = 0x77,
		SC_CONVERT = 0x79,   /* (Japanese keyboard)            */
		SC_NOCONVERT = 0x7B, /* (Japanese keyboard)            */

		SC_MEDIA_PREVIOUS = 0xE010,
		SC_MEDIA_NEXT = 0xE019,
		SC_NUMPAD_ENTER = 0xE01C,
		SC_RIGHT_CONTROL = 0xE01D,
		SC_VOLUME_MUTE = 0xE020,
		SC_LAUCNH_APP_2 = 0xE021,
		SC_MEDIA_PLAY = 0xE022,
		SC_MEDIA_STOP = 0xE024,
		SC_VOLUME_DOWN = 0xE02E,
		SC_VOLUME_UP = 0xE030,
		SC_BROWSER_HOME = 0xE032,
		SC_NUMPAD_DIVIDE = 0xE035,
		SC_PRINT_SCREEN = 0xE037, /* make: 0xE02A 0xE037 break: 0xE0B7 0xE0AA MapVirtualKeyEx( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) returns scancode 0x54; - There is no VK_KEYDOWN
		                             with VK_SNAPSHOT.*/

		SC_RIGHT_ALT = 0xE038,
		SC_CANCEL = 0xE046, /* CTRL + Pause */
		SC_HOME = 0xE047,
		SC_UP = 0xE048,
		SC_PAGE_UP = 0xE049,
		SC_LEFT = 0xE04B,
		SC_RIGHT = 0xE04D,
		SC_END = 0xE04F,
		SC_DOWN = 0xE050,
		SC_PAGE_DOWN = 0xE051,
		SC_INSERT = 0xE052,
		SC_DELETE = 0xE053,
		SC_LEFT_WINDOW = 0xE05B,
		SC_RIGHT_WINDOW = 0xE05C,
		SC_APPLICATION = 0xE05D,
		SC_POWER = 0xE05E,
		SC_SLEEP = 0xE05F,
		SC_WAKE = 0xE063,
		SC_BROWSER_SEARCH = 0xE065,
		SC_BROWSER_FAVORITES = 0xE066,
		SC_BROWSER_REFRESH = 0xE067,
		SC_BROWSER_STOP = 0xE068,
		SC_BROWSER_FORWARD = 0xE069,
		SC_BROWSER_BACK = 0xE06A,
		SC_LAUNCH_APP_1 = 0xE06B,
		SC_LAUNCH_EMAIL = 0xE06C,
		SC_LAUNCH_MEDIA = 0xE06D,

		// SC_PAUSE				= 0xE11D45, /* make: 0xE11D 45 0xE19D C5 make in raw input: 0xE11D 0x45 break: none No repeat when you hold the key down There are no break so I
		// don't know how the key down/up is expected to work. Raw input sends "keydown" and "keyup" messages, and it appears that the keyup message is sent directly after the
		// keydown message (you can't hold the key down) so depending on when GetMessage or PeekMessage will return messages, you may get both a keydown and keyup message "at the
		// same time". If you use VK messages most of the time you only get keydown messages, but some times you get keyup messages too. when pressed at the same time as one or
		// both control keys, generates a 0xE046 (sc_cancel) and the string for that scancode is "break". */

		SC_NONE = 0
	};

	/// @brief
	/// @param inputId
	/// @return
	constexpr int32_t InputIdLocalIndex(InputId inputId)
	{
		return std::to_underlying(inputId) - std::to_underlying(InputId::KeyboardStartEnum);
	}

	/// @brief
	/// @param inputId
	/// @return
	WINDOWS_SCAN_CODE ConvertInputIdToWindowsScanCode(InputId inputId)
	{
		static std::array<WINDOWS_SCAN_CODE, std::to_underlying(InputId::KeyboardEnumCount)> inputIdToWindowsScanCode;

		static bool mapped = false;
		if (mapped == false)
		{
			mapped = true;

			inputIdToWindowsScanCode.fill(WINDOWS_SCAN_CODE::SC_NONE);

			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyEscape)] = WINDOWS_SCAN_CODE::SC_ESCAPE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key1)] = WINDOWS_SCAN_CODE::SC_1;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key2)] = WINDOWS_SCAN_CODE::SC_2;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key3)] = WINDOWS_SCAN_CODE::SC_3;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key4)] = WINDOWS_SCAN_CODE::SC_4;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key5)] = WINDOWS_SCAN_CODE::SC_5;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key6)] = WINDOWS_SCAN_CODE::SC_6;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key7)] = WINDOWS_SCAN_CODE::SC_7;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key8)] = WINDOWS_SCAN_CODE::SC_8;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key9)] = WINDOWS_SCAN_CODE::SC_9;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::Key0)] = WINDOWS_SCAN_CODE::SC_0;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeySubtract)] = WINDOWS_SCAN_CODE::SC_SUBSTRACT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyEqual)] = WINDOWS_SCAN_CODE::SC_EQUAL;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyBackspace)] = WINDOWS_SCAN_CODE::SC_BACKSPACE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyTab)] = WINDOWS_SCAN_CODE::SC_TAB;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyQ)] = WINDOWS_SCAN_CODE::SC_Q;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyW)] = WINDOWS_SCAN_CODE::SC_W;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyE)] = WINDOWS_SCAN_CODE::SC_E;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyR)] = WINDOWS_SCAN_CODE::SC_R;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyT)] = WINDOWS_SCAN_CODE::SC_T;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyY)] = WINDOWS_SCAN_CODE::SC_Y;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyU)] = WINDOWS_SCAN_CODE::SC_U;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyI)] = WINDOWS_SCAN_CODE::SC_I;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyO)] = WINDOWS_SCAN_CODE::SC_O;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyP)] = WINDOWS_SCAN_CODE::SC_P;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeftBracket)] = WINDOWS_SCAN_CODE::SC_LEFT_BRACKET;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRightBracket)] = WINDOWS_SCAN_CODE::SC_RIGHT_BRACKET;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyEnter)] = WINDOWS_SCAN_CODE::SC_ENTER;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeftControl)] = WINDOWS_SCAN_CODE::SC_LEFT_CONTROL;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyA)] = WINDOWS_SCAN_CODE::SC_A;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyS)] = WINDOWS_SCAN_CODE::SC_S;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyD)] = WINDOWS_SCAN_CODE::SC_D;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF)] = WINDOWS_SCAN_CODE::SC_F;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyG)] = WINDOWS_SCAN_CODE::SC_G;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyH)] = WINDOWS_SCAN_CODE::SC_H;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyJ)] = WINDOWS_SCAN_CODE::SC_J;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyK)] = WINDOWS_SCAN_CODE::SC_K;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyL)] = WINDOWS_SCAN_CODE::SC_L;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeySemicolon)] = WINDOWS_SCAN_CODE::SC_SEMICOLON;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyApostrophe)] = WINDOWS_SCAN_CODE::SC_APOSTROPHE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyTilde)] = WINDOWS_SCAN_CODE::SC_TILDE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeftShift)] = WINDOWS_SCAN_CODE::SC_LEFT_SHIFT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyBackslash)] = WINDOWS_SCAN_CODE::SC_BACKSLASH;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyZ)] = WINDOWS_SCAN_CODE::SC_Z;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyX)] = WINDOWS_SCAN_CODE::SC_X;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyC)] = WINDOWS_SCAN_CODE::SC_C;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyV)] = WINDOWS_SCAN_CODE::SC_V;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyB)] = WINDOWS_SCAN_CODE::SC_B;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyN)] = WINDOWS_SCAN_CODE::SC_N;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyM)] = WINDOWS_SCAN_CODE::SC_M;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyComma)] = WINDOWS_SCAN_CODE::SC_COMMA;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyPeriod)] = WINDOWS_SCAN_CODE::SC_PERIOD;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeySlash)] = WINDOWS_SCAN_CODE::SC_SLASH;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRightShift)] = WINDOWS_SCAN_CODE::SC_RIGHT_SHIFT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadMultiply)] = WINDOWS_SCAN_CODE::SC_NUMPAD_MULTIPLY;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeftAlt)] = WINDOWS_SCAN_CODE::SC_LEFT_ALT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeySpace)] = WINDOWS_SCAN_CODE::SC_SPACE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyCapsLock)] = WINDOWS_SCAN_CODE::SC_CAPS_LOCK;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF1)] = WINDOWS_SCAN_CODE::SC_F1;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF2)] = WINDOWS_SCAN_CODE::SC_F2;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF3)] = WINDOWS_SCAN_CODE::SC_F3;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF4)] = WINDOWS_SCAN_CODE::SC_F4;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF5)] = WINDOWS_SCAN_CODE::SC_F5;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF6)] = WINDOWS_SCAN_CODE::SC_F6;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF7)] = WINDOWS_SCAN_CODE::SC_F7;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF8)] = WINDOWS_SCAN_CODE::SC_F8;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF9)] = WINDOWS_SCAN_CODE::SC_F9;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF10)] = WINDOWS_SCAN_CODE::SC_F10;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumLock)] = WINDOWS_SCAN_CODE::SC_NUM_LOCK;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScrollLock)] = WINDOWS_SCAN_CODE::SC_SCROLL_LOCK;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad7)] = WINDOWS_SCAN_CODE::SC_NUMPAD_7;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad8)] = WINDOWS_SCAN_CODE::SC_NUMPAD_8;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad9)] = WINDOWS_SCAN_CODE::SC_NUMPAD_9;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadSubtract)] = WINDOWS_SCAN_CODE::SC_NUMPAD_SUBTRACT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad4)] = WINDOWS_SCAN_CODE::SC_NUMPAD_4;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad5)] = WINDOWS_SCAN_CODE::SC_NUMPAD_5;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad6)] = WINDOWS_SCAN_CODE::SC_NUMPAD_6;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadAdd)] = WINDOWS_SCAN_CODE::SC_NUMPAD_ADD;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad1)] = WINDOWS_SCAN_CODE::SC_NUMPAD_1;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad2)] = WINDOWS_SCAN_CODE::SC_NUMPAD_2;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad3)] = WINDOWS_SCAN_CODE::SC_NUMPAD_3;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpad0)] = WINDOWS_SCAN_CODE::SC_NUMPAD_0;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadDecimal)] = WINDOWS_SCAN_CODE::SC_NUMPAD_DECIMAL;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeySysRq)] = WINDOWS_SCAN_CODE::SC_SYS_RQ;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode85)] = WINDOWS_SCAN_CODE::SC_NONE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyUnlabelled)] = WINDOWS_SCAN_CODE::SC_NONE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF11)] = WINDOWS_SCAN_CODE::SC_F11;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF12)] = WINDOWS_SCAN_CODE::SC_F12;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF13)] = WINDOWS_SCAN_CODE::SC_F13;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF14)] = WINDOWS_SCAN_CODE::SC_F14;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF15)] = WINDOWS_SCAN_CODE::SC_F15;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF16)] = WINDOWS_SCAN_CODE::SC_F16;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF17)] = WINDOWS_SCAN_CODE::SC_F17;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF18)] = WINDOWS_SCAN_CODE::SC_F18;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF19)] = WINDOWS_SCAN_CODE::SC_F19;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF20)] = WINDOWS_SCAN_CODE::SC_F20;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF21)] = WINDOWS_SCAN_CODE::SC_F21;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF22)] = WINDOWS_SCAN_CODE::SC_F22;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF23)] = WINDOWS_SCAN_CODE::SC_F23;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyF24)] = WINDOWS_SCAN_CODE::SC_F24;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem1)] = WINDOWS_SCAN_CODE::SC_OEM_1;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem2)] = WINDOWS_SCAN_CODE::SC_OEM_2;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem3)] = WINDOWS_SCAN_CODE::SC_OEM_3;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem4)] = WINDOWS_SCAN_CODE::SC_OEM_4;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem5)] = WINDOWS_SCAN_CODE::SC_OEM_5;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem6)] = WINDOWS_SCAN_CODE::SC_OEM_6;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyOem7)] = WINDOWS_SCAN_CODE::SC_OEM_7;
			/*
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode108)] = WINDOWS_SCAN_CODE::SC_OEM_102;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode109)] = WINDOWS_SCAN_CODE::SC_ERASE_EOF;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode110)] = WINDOWS_SCAN_CODE::SC_ZOOM_FUNC;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode111)] = WINDOWS_SCAN_CODE::SC_HELP;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode112)] = WINDOWS_SCAN_CODE::SC_KATAKANA;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode113)] = WINDOWS_SCAN_CODE::SC_ABNT_C1;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode114)] = WINDOWS_SCAN_CODE::SC_SBCSCHAR;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode115)] = WINDOWS_SCAN_CODE::SC_CONVERT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyScancode116)] = WINDOWS_SCAN_CODE::SC_NOCONVERT;
			*/

			// Escaped
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadEnter)] = WINDOWS_SCAN_CODE::SC_NUMPAD_ENTER;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyNumpadDivide)] = WINDOWS_SCAN_CODE::SC_NUMPAD_DIVIDE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRightControl)] = WINDOWS_SCAN_CODE::SC_RIGHT_CONTROL;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRightAlt)] = WINDOWS_SCAN_CODE::SC_RIGHT_ALT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyPrintScreen)] = WINDOWS_SCAN_CODE::SC_PRINT_SCREEN;
			// inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyBreak)] = WINDOWS_SCAN_CODE::SC_PAUSE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyUp)] = WINDOWS_SCAN_CODE::SC_UP;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeft)] = WINDOWS_SCAN_CODE::SC_LEFT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRight)] = WINDOWS_SCAN_CODE::SC_RIGHT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyDown)] = WINDOWS_SCAN_CODE::SC_DOWN;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyPageUp)] = WINDOWS_SCAN_CODE::SC_PAGE_UP;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyPageDown)] = WINDOWS_SCAN_CODE::SC_PAGE_DOWN;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyHome)] = WINDOWS_SCAN_CODE::SC_HOME;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyEnd)] = WINDOWS_SCAN_CODE::SC_END;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyInsert)] = WINDOWS_SCAN_CODE::SC_INSERT;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyDelete)] = WINDOWS_SCAN_CODE::SC_DELETE;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyLeftWindow)] = WINDOWS_SCAN_CODE::SC_LEFT_WINDOW;
			inputIdToWindowsScanCode[InputIdLocalIndex(InputId::KeyRightWindow)] = WINDOWS_SCAN_CODE::SC_RIGHT_WINDOW;
			// inputIdToWindowsScanCode[InputId::KeyMENU] =

			/* Unampped
			    SC_MEDIA_PREVIOUS
			    SC_MEDIA_NEXT

			    SC_VOLUME_MUTE
			    SC_LAUCNH_APP_2
			    SC_MEDIA_PLAY
			    SC_MEDIA_STOP
			    SC_VOLUME_DOWN
			    SC_VOLUME_UP
			    SC_BROWSER_HOME

			    SC_CANCEL

			    SC_APPLICATION
			    SC_POWER
			    SC_SLEEP
			    SC_WAKE
			    SC_BROWSER_SEARCH
			    SC_BROWSER_FAVORITES
			    SC_BROWSER_REFRESH
			    SC_BROWSER_STOP
			    SC_BROWSER_FORWARD
			    SC_BROWSER_BACK
			    SC_LAUNCH_APP_1
			    SC_LAUNCH_EMAIL
			    SC_LAUNCH_MEDIA
			*/
		}

		return inputIdToWindowsScanCode[InputIdLocalIndex(inputId)];
	}

	/// @brief
	/// @param scanCode
	/// @return
	InputId ConvertWindowsScanCodeToInputId(WINDOWS_SCAN_CODE scanCode)
	{
		static std::array<InputId, std::numeric_limits<std::underlying_type_t<WINDOWS_SCAN_CODE>>().max()> windowsScanCodeToInputId;

		static bool mapped = false;
		if (mapped == false)
		{
			mapped = true;

			windowsScanCodeToInputId.fill(InputId::Unknown);

			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_ESCAPE)] = InputId::KeyEscape;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_1)] = InputId::Key1;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_2)] = InputId::Key2;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_3)] = InputId::Key3;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_4)] = InputId::Key4;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_5)] = InputId::Key5;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_6)] = InputId::Key6;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_7)] = InputId::Key7;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_8)] = InputId::Key8;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_9)] = InputId::Key9;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_0)] = InputId::Key0;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SUBSTRACT)] = InputId::KeySubtract;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_EQUAL)] = InputId::KeyEqual;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_BACKSPACE)] = InputId::KeyBackspace;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_TAB)] = InputId::KeyTab;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_Q)] = InputId::KeyQ;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_W)] = InputId::KeyW;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_E)] = InputId::KeyE;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_R)] = InputId::KeyR;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_T)] = InputId::KeyT;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_Y)] = InputId::KeyY;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_U)] = InputId::KeyU;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_I)] = InputId::KeyI;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_O)] = InputId::KeyO;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_P)] = InputId::KeyP;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT_BRACKET)] = InputId::KeyLeftBracket;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT_BRACKET)] = InputId::KeyRightBracket;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_ENTER)] = InputId::KeyEnter;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT_CONTROL)] = InputId::KeyLeftControl;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_A)] = InputId::KeyA;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_S)] = InputId::KeyS;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_D)] = InputId::KeyD;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F)] = InputId::KeyF;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_G)] = InputId::KeyG;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_H)] = InputId::KeyH;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_J)] = InputId::KeyJ;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_K)] = InputId::KeyK;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_L)] = InputId::KeyL;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SEMICOLON)] = InputId::KeySemicolon;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_APOSTROPHE)] = InputId::KeyApostrophe;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_TILDE)] = InputId::KeyTilde;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT_SHIFT)] = InputId::KeyLeftShift;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_BACKSLASH)] = InputId::KeyBackslash;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_Z)] = InputId::KeyZ;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_X)] = InputId::KeyX;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_C)] = InputId::KeyC;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_V)] = InputId::KeyV;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_B)] = InputId::KeyB;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_N)] = InputId::KeyN;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_M)] = InputId::KeyM;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_COMMA)] = InputId::KeyComma;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_PERIOD)] = InputId::KeyPeriod;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SLASH)] = InputId::KeySlash;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT_SHIFT)] = InputId::KeyRightShift;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_MULTIPLY)] = InputId::KeyNumpadMultiply;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT_ALT)] = InputId::KeyLeftAlt;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SPACE)] = InputId::KeySpace;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_CAPS_LOCK)] = InputId::KeyCapsLock;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F1)] = InputId::KeyF1;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F2)] = InputId::KeyF2;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F3)] = InputId::KeyF3;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F4)] = InputId::KeyF4;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F5)] = InputId::KeyF5;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F6)] = InputId::KeyF6;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F7)] = InputId::KeyF7;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F8)] = InputId::KeyF8;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F9)] = InputId::KeyF9;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F10)] = InputId::KeyF10;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUM_LOCK)] = InputId::KeyNumLock;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCROLL_LOCK)] = InputId::KeyScrollLock;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_7)] = InputId::KeyNumpad7;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_8)] = InputId::KeyNumpad8;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_9)] = InputId::KeyNumpad9;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_SUBTRACT)] = InputId::KeyNumpadSubtract;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_4)] = InputId::KeyNumpad4;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_5)] = InputId::KeyNumpad5;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_6)] = InputId::KeyNumpad6;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_ADD)] = InputId::KeyNumpadAdd;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_1)] = InputId::KeyNumpad1;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_2)] = InputId::KeyNumpad2;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_3)] = InputId::KeyNumpad3;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_0)] = InputId::KeyNumpad0;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_DECIMAL)] = InputId::KeyNumpadDecimal;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SYS_RQ)] = InputId::KeySysRq;
			// windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_85)] = InputId::KeyNONE;
			// windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_UNLABELLED)] = InputId::KeyNONE;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F11)] = InputId::KeyF11;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F12)] = InputId::KeyF12;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F13)] = InputId::KeyF13;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F14)] = InputId::KeyF14;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F15)] = InputId::KeyF15;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F16)] = InputId::KeyF16;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F17)] = InputId::KeyF17;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F18)] = InputId::KeyF18;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F19)] = InputId::KeyF19;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F20)] = InputId::KeyF20;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F21)] = InputId::KeyF21;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F22)] = InputId::KeyF22;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F23)] = InputId::KeyF23;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_F24)] = InputId::KeyF24;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem1;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem2;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem3;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem4;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem5;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem6;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_OEM_1)] = InputId::KeyOem7;
			/*
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_108)] = InputId::KeyOEM_102;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_109)] = InputId::KeyERASE_EOF;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_110)] = InputId::KeyZOOM_FUNC;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_111)] = InputId::KeyHELP;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_112)] = InputId::KeyKATAKANA;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_113)] = InputId::KeyABNT_C1;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_114)] = InputId::KeySBCSCHAR;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_115)] = InputId::KeyCONVERT;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_SCANCODE_116)] = InputId::KeyNOCONVERT;
			*/

			// Escaped
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_ENTER)] = InputId::KeyNumpadEnter;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_NUMPAD_DIVIDE)] = InputId::KeyNumpadDivide;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT_CONTROL)] = InputId::KeyRightControl;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT_ALT)] = InputId::KeyRightAlt;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_PRINT_SCREEN)] = InputId::KeyPrintScreen;
			// windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_BREAK)] = InputId::KeyBreak;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_UP)] = InputId::KeyUp;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT)] = InputId::KeyLeft;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT)] = InputId::KeyRight;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_DOWN)] = InputId::KeyDown;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_PAGE_UP)] = InputId::KeyPageUp;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_PAGE_DOWN)] = InputId::KeyPageDown;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_HOME)] = InputId::KeyHome;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_END)] = InputId::KeyEnd;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_INSERT)] = InputId::KeyInsert;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_DELETE)] = InputId::KeyDelete;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_LEFT_WINDOW)] = InputId::KeyLeftWindow;
			windowsScanCodeToInputId[std::to_underlying(WINDOWS_SCAN_CODE::SC_RIGHT_WINDOW)] = InputId::KeyRightWindow;
			// windowsScanCodeToInputId[InputId::KeyMENU] =

			/* Unampped
			    SC_MEDIA_PREVIOUS
			    SC_MEDIA_NEXT

			    SC_VOLUME_MUTE
			    SC_LAUCNH_APP_2
			    SC_MEDIA_PLAY
			    SC_MEDIA_STOP
			    SC_VOLUME_DOWN
			    SC_VOLUME_UP
			    SC_BROWSER_HOME

			    SC_CANCEL

			    SC_APPLICATION
			    SC_POWER
			    SC_SLEEP
			    SC_WAKE
			    SC_BROWSER_SEARCH
			    SC_BROWSER_FAVORITES
			    SC_BROWSER_REFRESH
			    SC_BROWSER_STOP
			    SC_BROWSER_FORWARD
			    SC_BROWSER_BACK
			    SC_LAUNCH_APP_1
			    SC_LAUNCH_EMAIL
			    SC_LAUNCH_MEDIA
			*/
		}

		return windowsScanCodeToInputId[std::to_underlying(scanCode)];
	}

	/// @brief
	/// @param virtualKey
	/// @return
	WINDOWS_SCAN_CODE ConvertVirtualKeyToWindowsScanCode(UINT virtualKey)
	{
		switch (virtualKey)
		{
			case VK_UP: return WINDOWS_SCAN_CODE::SC_UP;
			case VK_DOWN: return WINDOWS_SCAN_CODE::SC_DOWN;
			case VK_LEFT: return WINDOWS_SCAN_CODE::SC_LEFT;
			case VK_RIGHT: return WINDOWS_SCAN_CODE::SC_RIGHT;
			case VK_INSERT: return WINDOWS_SCAN_CODE::SC_INSERT;
			case VK_DELETE: return WINDOWS_SCAN_CODE::SC_DELETE;
			case VK_PRIOR: return WINDOWS_SCAN_CODE::SC_PAGE_UP;
			case VK_NEXT: return WINDOWS_SCAN_CODE::SC_PAGE_DOWN;
			case VK_HOME: return WINDOWS_SCAN_CODE::SC_HOME;
			case VK_END: return WINDOWS_SCAN_CODE::SC_END;
			case VK_LMENU: return WINDOWS_SCAN_CODE::SC_LEFT_ALT;
			case VK_RMENU: return WINDOWS_SCAN_CODE::SC_RIGHT_ALT;
			case VK_LCONTROL: return WINDOWS_SCAN_CODE::SC_LEFT_CONTROL;
			case VK_RCONTROL: return WINDOWS_SCAN_CODE::SC_RIGHT_CONTROL;
			case VK_LSHIFT: return WINDOWS_SCAN_CODE::SC_LEFT_SHIFT;
			case VK_RSHIFT: return WINDOWS_SCAN_CODE::SC_RIGHT_SHIFT;
			case VK_LWIN: return WINDOWS_SCAN_CODE::SC_LEFT_WINDOW;
			case VK_RWIN: return WINDOWS_SCAN_CODE::SC_RIGHT_WINDOW;
			case VK_DECIMAL: return WINDOWS_SCAN_CODE::SC_NUMPAD_DECIMAL;
			case VK_NUMPAD0: return WINDOWS_SCAN_CODE::SC_NUMPAD_0;
			case VK_NUMPAD1: return WINDOWS_SCAN_CODE::SC_NUMPAD_1;
			case VK_NUMPAD2: return WINDOWS_SCAN_CODE::SC_NUMPAD_2;
			case VK_NUMPAD3: return WINDOWS_SCAN_CODE::SC_NUMPAD_3;
			case VK_NUMPAD4: return WINDOWS_SCAN_CODE::SC_NUMPAD_4;
			case VK_NUMPAD5: return WINDOWS_SCAN_CODE::SC_NUMPAD_5;
			case VK_NUMPAD6: return WINDOWS_SCAN_CODE::SC_NUMPAD_6;
			case VK_NUMPAD7: return WINDOWS_SCAN_CODE::SC_NUMPAD_7;
			case VK_NUMPAD8: return WINDOWS_SCAN_CODE::SC_NUMPAD_8;
			case VK_NUMPAD9: return WINDOWS_SCAN_CODE::SC_NUMPAD_9;
			// case VK_RETURN: return WINDOWS_SCAN_CODE::SC_ENTER;
			default:
			{
				// TODO Get windows application
				HKL hKeyboardLayout = GetKeyboardLayout(0);

				return static_cast<WINDOWS_SCAN_CODE>(MapVirtualKeyEx(virtualKey, MAPVK_VK_TO_VSC, hKeyboardLayout));
			}
		}
	}

	/// @brief
	/// @param virtualKey
	/// @return
	UINT ConvertWindowsScanCodeToVirtualKey(WINDOWS_SCAN_CODE scanCode)
	{
		switch (scanCode)
		{
			case WINDOWS_SCAN_CODE::SC_UP: return VK_UP;
			case WINDOWS_SCAN_CODE::SC_DOWN: return VK_DOWN;
			case WINDOWS_SCAN_CODE::SC_LEFT: return VK_LEFT;
			case WINDOWS_SCAN_CODE::SC_RIGHT: return VK_RIGHT;
			case WINDOWS_SCAN_CODE::SC_INSERT: return VK_INSERT;
			case WINDOWS_SCAN_CODE::SC_DELETE: return VK_DELETE;
			case WINDOWS_SCAN_CODE::SC_PAGE_UP: return VK_PRIOR;
			case WINDOWS_SCAN_CODE::SC_PAGE_DOWN: return VK_NEXT;
			case WINDOWS_SCAN_CODE::SC_HOME: return VK_HOME;
			case WINDOWS_SCAN_CODE::SC_END: return VK_END;
			case WINDOWS_SCAN_CODE::SC_LEFT_ALT: return VK_LMENU;
			case WINDOWS_SCAN_CODE::SC_RIGHT_ALT: return VK_RMENU;
			case WINDOWS_SCAN_CODE::SC_LEFT_CONTROL: return VK_LCONTROL;
			case WINDOWS_SCAN_CODE::SC_RIGHT_CONTROL: return VK_RCONTROL;
			case WINDOWS_SCAN_CODE::SC_LEFT_SHIFT: return VK_LSHIFT;
			case WINDOWS_SCAN_CODE::SC_RIGHT_SHIFT: return VK_RSHIFT;
			case WINDOWS_SCAN_CODE::SC_LEFT_WINDOW: return VK_LWIN;
			case WINDOWS_SCAN_CODE::SC_RIGHT_WINDOW: return VK_RWIN;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_DECIMAL: return VK_DECIMAL;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_0: return VK_NUMPAD0;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_1: return VK_NUMPAD1;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_2: return VK_NUMPAD2;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_3: return VK_NUMPAD3;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_4: return VK_NUMPAD4;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_5: return VK_NUMPAD5;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_6: return VK_NUMPAD6;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_7: return VK_NUMPAD7;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_8: return VK_NUMPAD8;
			case WINDOWS_SCAN_CODE::SC_NUMPAD_9: return VK_NUMPAD9;
			// case WINDOWS_SCAN_CODE::SC_ENTER: return VK_RETURN;
			default:
			{
				// TODO Get windows application
				HKL hKeyboardLayout = GetKeyboardLayout(0);

				return MapVirtualKeyEx(static_cast<UINT>(scanCode), MAPVK_VSC_TO_VK, hKeyboardLayout);
			}
		}
	}

	/// @brief
	/// @param handle
	/// @param name
	/// @param info
	KeyboardRawInput::KeyboardRawInput(HANDLE handle, const std::string_view& name, const tagRID_DEVICE_INFO_KEYBOARD& /*info*/)
	: Keyboard(ComputeDeviceUID(handle), name, Product::UNKNOWN)
	, _handle(handle)
	{
		/*
		for (uint32_t index = 0; index < InputId::KeyboardEnumCount; ++index)
		{
		    AddInput(&_keys[index]);
		}
		for (uint32_t index = 0; index < InputId::KeyboardVirtualEnumCount; ++index)
		{
		    AddInput(&_virtualKeys[index]);
		}
		*/
		SetConnected(true);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool KeyboardRawInput::ApplyFeedback(Feedback& /*feedback*/)
	{
		return false;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void KeyboardRawInput::ReadRawInput(const tagRAWKEYBOARD& rawKeyboard)
	{
		WINDOWS_SCAN_CODE scanCode = static_cast<WINDOWS_SCAN_CODE>(rawKeyboard.MakeCode);
		if (rawKeyboard.Flags & RI_KEY_E0)
		{
			scanCode = static_cast<WINDOWS_SCAN_CODE>(0xe000 | rawKeyboard.MakeCode);
		}

		InputId inputId = ConvertWindowsScanCodeToInputId(scanCode);
		if (inputId != InputId::Unknown)
		{
			// float value = (rawKeyboard.Flags & RI_KEY_BREAK) ? 0.0f : 1.0f;

			// int32_t index = static_cast<int32_t>(inputId) - (InputId::KeyboardStartEnum + 1);

			//			SetInputValue(_keys[index], value);

			InputId physicalInputId = inputId;
			InputId virtualInputId = InputIdHelper::PhysicalToVirtual(physicalInputId);
			if (physicalInputId != virtualInputId)
			{
				//				SetInputValue(_virtualKeys[static_cast<uint32_t>(virtualInputId) - static_cast<uint32_t>(InputId::KeyboardVirtualStartEnum) - 1], value);
			}
		}
	}

	/// @brief
	/// @return
	HANDLE KeyboardRawInput::GetHandle() const
	{
		return _handle;
	}

	/// @brief
	/// @param hDevice
	/// @return
	UID KeyboardRawInput::ComputeDeviceUID(HANDLE hDevice)
	{
		return UID(Api::DeviceUidOffset::RAW_INPUT, reinterpret_cast<uint64_t>(hDevice));
	}
}
