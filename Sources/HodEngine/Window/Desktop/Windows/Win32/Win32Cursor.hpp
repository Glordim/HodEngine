#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Desktop/Cursor.hpp"

struct HICON__;
using HCURSOR = HICON__*;

namespace hod::window
{
	/// @brief
	class HOD_WINDOW_API Win32Cursor : public Cursor
	{
	public:
		Win32Cursor(HCURSOR cursorHandle);

		HCURSOR GetCursorHandle() const;

	private:
		HCURSOR _cursorHandle;
	};
}
