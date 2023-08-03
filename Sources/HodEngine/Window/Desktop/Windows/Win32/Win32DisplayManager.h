#pragma once

#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Core/Singleton.h"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.h"

namespace hod::window
{
    class Win32DisplayManager : public DesktopDisplayManager
    {
        _Singleton(Win32DisplayManager)

    public:

        bool    Initialize() override;

        Window* CreateWindow() override;
        void    DestroyWindow(Window* window) override;
    };
}

#endif
