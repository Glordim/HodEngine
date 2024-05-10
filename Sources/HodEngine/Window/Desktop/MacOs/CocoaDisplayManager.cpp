#if defined(PLATFORM_MACOS)

#include "HodEngine/Window/Desktop/MacOs/CocoaDisplayManager.hpp"
#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::window
{
    _SingletonConstructor(CocoaDisplayManager)
    : DesktopDisplayManager()
    {

    }

    /// @brief 
    /// @return 
    bool CocoaDisplayManager::Initialize()
    {
        return true;
    }

    /// @brief 
    void CocoaDisplayManager::Terminate()
    {
    }

    /// @brief 
    /// @return 
    Window* CocoaDisplayManager::CreateWindow()
    {
        return new MacOsWindow();
    }

    /// @brief 
    /// @param window 
    void CocoaDisplayManager::DestroyWindow(Window* window)
    {
        delete window;
    }
}

#endif
