#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/MacOs/CocoaDisplayManager.hpp"
#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::inline window
{
    _SingletonOverrideConstructor(CocoaDisplayManager)
    : DesktopDisplayManager()
    {

    }

    /// @brief 
    /// @return 
    bool CocoaDisplayManager::Initialize()
    {
        CocoaInit();
        _mainWindow = CreateWindow();
        return true;
    }

    /// @brief 
    void CocoaDisplayManager::Terminate()
    {
    }

    /// @brief 
    /// @return 
    Window* CocoaDisplayManager::CreateWindow(bool hidden)
    {
        MacOsWindow* window = DefaultAllocator::GetInstance().New<MacOsWindow>(hidden);
        _windows.PushBack(window);
        return window;
    }

    /// @brief 
    /// @param window 
    void CocoaDisplayManager::DestroyWindow(Window* window)
    {
        DefaultAllocator::GetInstance().Delete(window);
    }

    Cursor* CocoaDisplayManager::CreateBuiltinCursor(BuiltinCursor /*builtinCursor*/)
    {
        return nullptr;
    }
}
