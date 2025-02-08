#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32DisplayManager.hpp"
#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/OS.hpp>

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

        _hInstance = ::GetModuleHandle(NULL);

		WNDCLASSEX windowClass;
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = &Win32Window::WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = _hInstance;
		windowClass.hIcon = NULL;
		windowClass.hCursor = ::LoadCursor(_hInstance, IDC_ARROW);
		windowClass.hbrBackground = ::CreateSolidBrush(RGB(0, 0, 0));
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = _className;
		windowClass.hIconSm = NULL;

		_class = ::RegisterClassEx(&windowClass);
		if (_class == INVALID_ATOM)
		{
			OUTPUT_ERROR("Win32DisplayManager: Unable to RegisterClass -> {}", OS::GetLastWin32ErrorMessage());
			return false;
		}

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
        if (_class != INVALID_ATOM)
		{
			::UnregisterClass(_className, _hInstance);
            _class = INVALID_ATOM;
		}

        _hInstance = NULL;

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
