#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.h"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.h"

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
        if (OleInitialize(nullptr) != S_OK)
        {
            // todo output
            return false;
        }
        return true;
    }

    /// @brief 
    void Win32DisplayManager::Terminate()
    {
        OleUninitialize();
    }

    /// @brief 
    /// @return 
    Window* Win32DisplayManager::CreateWindow()
    {
        return new Win32Window();
    }

    /// @brief 
    /// @param window 
    void Win32DisplayManager::DestroyWindow(Window* window)
    {
        delete window;
    }
}

#endif
