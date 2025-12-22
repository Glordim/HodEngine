#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/MacOs/CocoaDisplayManager.hpp"
#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

namespace hod::window
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

    void CocoaDisplayManager::Update()
    {

    }

    /// @brief 
    void CocoaDisplayManager::Terminate()
    {
    }

    /// @brief 
    /// @return 
    Window* CocoaDisplayManager::CreateWindow(bool hidden)
    {
        return DefaultAllocator::GetInstance().New<MacOsWindow>(hidden);
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
