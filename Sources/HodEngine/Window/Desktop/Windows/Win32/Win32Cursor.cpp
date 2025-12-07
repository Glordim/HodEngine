#include "HodEngine/Window/Pch.hpp"
#include "Win32Cursor.hpp"

namespace hod::window
{
	Win32Cursor::Win32Cursor(HCURSOR cursorHandle)
	: _cursorHandle(cursorHandle)
	{
	}

	HCURSOR Win32Cursor::GetCursorHandle() const
	{
		return _cursorHandle;
	}
}
