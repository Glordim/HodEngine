#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.h"

#include <ole2.h>

namespace hod::window
{
    /// @brief 
    /// @return 
    bool Win32DisplayManager::Initialize()
    {
        OleInitialize(nullptr);
    }

    /// @brief 
    /// @return 
    Window* Win32DisplayManager::CreateWindow()
    {
        return nullptr;
    }

    /// @brief 
    /// @param window 
    void Win32DisplayManager::DestroyWindow(Window* window)
    {

    }
}

#endif
