#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"

#include <ole2.h>

#undef CreateWindow

namespace hod::window
{
    _SingletonOverrideConstructor(Win32DisplayManager)
    : DesktopDisplayManager()
    {

    }

    /// @brief 
    /// @return 
    bool Win32DisplayManager::Initialize()
    {
        OleInitialize(nullptr);

        _mainWindow = CreateWindow();

        return true;
    }

    /// @brief 
    void Win32DisplayManager::Update()
    {
        for (Window* window : _windows)
        {
            window->Update();
        }
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
        Window* window = new Win32Window(hidden);
        _windows.push_back(window);
        return window;
    }

    /// @brief 
    /// @param window 
    void Win32DisplayManager::DestroyWindow(Window* window)
    {
        auto it = std::find(_windows.begin(), _windows.end(), window);
        if (it != _windows.end())
        {
            _windows.erase(it);
        }
        delete window;
    }
}

#endif
