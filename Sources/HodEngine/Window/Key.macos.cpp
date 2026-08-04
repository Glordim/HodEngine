#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Key.hpp"

#include <Carbon/Carbon.h>

namespace hod::inline window
{
	// Letters and punctuation shift meaning with the active keyboard layout
	// (e.g. the physical key at the QWERTY "W" position types 'z' on
	// AZERTY); everything else (arrows, Enter, function keys, modifiers,
	// numpad, ...) means the same thing on every layout, so ScanCodeToKey()
	// already handles it correctly without asking the OS.
	static bool IsLayoutDependent(ScanCode scanCode)
	{
		uint8_t value = static_cast<uint8_t>(scanCode);
		return value >= static_cast<uint8_t>(ScanCode::A) && value <= static_cast<uint8_t>(ScanCode::Z);
	}

	// Translates a macOS virtual keyCode into the character it currently
	// produces, per the active keyboard layout. The dead-key state is
	// always reset and kUCKeyTranslateNoDeadKeysBit is set, so this never
	// waits on (or is affected by) a live dead-key/IME composition: a dead
	// key alone resolves to its standalone spacing glyph (´, `, ^, ...)
	// instead of Key::None. Composed text itself is reported separately,
	// asynchronously, through NSTextInputClient's insertText:.
	static UniChar TranslateKeyCodeToChar(uint16_t keyCode)
	{
		TISInputSourceRef source = TISCopyCurrentKeyboardLayoutInputSource();
		if (source == nullptr)
		{
			return 0;
		}

		CFDataRef layoutData = static_cast<CFDataRef>(
			TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));
		if (layoutData == nullptr)
		{
			CFRelease(source);
			return 0;
		}

		const UCKeyboardLayout* keyboardLayout =
			reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layoutData));

		UInt32 deadKeyState = 0;
		UniChar chars[4] = {0};
		UniCharCount actualLength = 0;

		OSStatus status = UCKeyTranslate(
			keyboardLayout,
			keyCode,
			kUCKeyActionDown,
			0, // no modifiers: base/unshifted layer
			LMGetKbdType(),
			kUCKeyTranslateNoDeadKeysBit,
			&deadKeyState,
			sizeof(chars) / sizeof(chars[0]),
			&actualLength,
			chars);

		CFRelease(source);

		if (status != noErr || actualLength == 0)
		{
			return 0;
		}
		return chars[0];
	}

	static Key CharToKey(UniChar c)
	{
		// Key::A..Key::Z are contiguous, so the offset from 'a'/'A' maps
		// directly onto the enum range.
		if (c >= 'a' && c <= 'z')
		{
			return static_cast<Key>(static_cast<uint8_t>(Key::A) + (c - 'a'));
		}
		if (c >= 'A' && c <= 'Z')
		{
			return static_cast<Key>(static_cast<uint8_t>(Key::A) + (c - 'A'));
		}
		return Key::None;
	}

	Key MacOSKeyCodeToKey(uint16_t keyCode)
	{
		ScanCode scanCode = MacOSKeyCodeToScanCode(keyCode);

		if (IsLayoutDependent(scanCode))
		{
			Key key = CharToKey(TranslateKeyCodeToChar(keyCode));
			if (key != Key::None)
			{
				return key;
			}
		}

		return ScanCodeToKey(scanCode);
	}
}
