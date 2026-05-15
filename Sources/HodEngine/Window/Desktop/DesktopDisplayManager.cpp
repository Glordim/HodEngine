#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"
#include "HodEngine/Window/Desktop/Cursor.hpp"

namespace hod::inline window
{
	_SingletonOverrideConstructor(DesktopDisplayManager)
	: DisplayManager()
	{
	}

	DesktopDisplayManager::~DesktopDisplayManager()
	{
		for (Cursor* builtinCursor : _builtinCursors)
		{
			DefaultAllocator::GetInstance().Delete(builtinCursor);
		}
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
