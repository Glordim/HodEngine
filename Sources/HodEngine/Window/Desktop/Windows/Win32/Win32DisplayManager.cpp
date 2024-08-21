#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"

#include <ole2.h>

#undef CreateWindow

namespace hod::window
{
    _SingletonConstructor(Win32DisplayManager)
    : DesktopDisplayManager()
    {

    }

    /// @brief 
    /// @return 
    bool Win32DisplayManager::Initialize()
    {
        OleInitialize(nullptr);
        return true;
    }

    /// @brief 
    void Win32DisplayManager::Terminate()
    {
        OleUninitialize();
    }

    /// @brief 
    /// @return 
    Window* Win32DisplayManager::CreateWindow(bool hidden)
    {
        return new Win32Window(hidden);
    }

    /// @brief 
    /// @param window 
    void Win32DisplayManager::DestroyWindow(Window* window)
    {
        delete window;
    }
}

#endif
