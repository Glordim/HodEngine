#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
    class HOD_API Win32DisplayManager : public DesktopDisplayManager
    {
        _Singleton(Win32DisplayManager)

    public:

        bool            Initialize() override;
        void            Terminate() override;

        Window*         CreateWindow(bool hidden = false) override;
        void            DestroyWindow(Window* window) override;
    };
}

#endif
