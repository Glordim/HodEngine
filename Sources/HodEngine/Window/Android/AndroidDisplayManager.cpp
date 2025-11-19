#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Android/AndroidDisplayManager.hpp"
#include "HodEngine/Window/Android/AndroidWindow.hpp"

namespace hod::window
{
    _SingletonOverrideConstructor(AndroidDisplayManager) : DisplayManager()
	{

	}

	/// @brief 
	/// @param nativeWindow 
	/// @return 
	Window* AndroidDisplayManager::CreateMainWindow(ANativeWindow* nativeWindow)
	{
		_mainWindow = DefaultAllocator::GetInstance().New<AndroidWindow>(nativeWindow);
		return _mainWindow;
	}
}
