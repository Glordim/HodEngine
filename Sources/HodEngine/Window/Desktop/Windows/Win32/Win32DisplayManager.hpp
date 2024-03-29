#pragma once

#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
    class Win32DisplayManager : public DesktopDisplayManager
    {
        _Singleton(Win32DisplayManager)

    public:

        bool            Initialize() override;
        void            Terminate() override;

        Window*         CreateWindow() override;
        void            DestroyWindow(Window* window) override;
    };
}

#endif
