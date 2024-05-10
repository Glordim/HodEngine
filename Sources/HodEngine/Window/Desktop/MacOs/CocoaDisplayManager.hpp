#pragma once

#if defined(PLATFORM_MACOS)

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
    class CocoaDisplayManager : public DesktopDisplayManager
    {
        _Singleton(CocoaDisplayManager)

    public:

        bool            Initialize() override;
        void            Terminate() override;

        Window*         CreateWindow() override;
        void            DestroyWindow(Window* window) override;
    };
}

#endif
