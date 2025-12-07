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
		Cursor* cursor = _builtinCursors[std::to_underlying(builtinCursor)];
		if (cursor == nullptr)
		{
			cursor = CreateBuiltinCursor(builtinCursor);
			_builtinCursors[std::to_underlying(builtinCursor)] = cursor;
		}
		return cursor;
	}
}
