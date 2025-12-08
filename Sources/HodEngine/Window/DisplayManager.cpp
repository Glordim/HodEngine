#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/DisplayManager.hpp"

namespace hod::window
{
	_SingletonConstructor(DisplayManager) {}

	Window* DisplayManager::GetMainWindow() const
	{
		return _mainWindow;
	}
}
