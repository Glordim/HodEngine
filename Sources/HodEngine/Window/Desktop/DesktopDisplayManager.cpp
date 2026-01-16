#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
	_SingletonOverrideConstructor(DesktopDisplayManager)
	: DisplayManager()
	{
	}

	Cursor* DesktopDisplayManager::GetBultinCursor(BuiltinCursor builtinCursor)
	{
		Cursor* cursor = _builtinCursors[static_cast<uint32_t>(builtinCursor)];
		if (cursor == nullptr)
		{
			cursor = CreateBuiltinCursor(builtinCursor);
			_builtinCursors[static_cast<uint32_t>(builtinCursor)] = cursor;
		}
		return cursor;
	}
}
