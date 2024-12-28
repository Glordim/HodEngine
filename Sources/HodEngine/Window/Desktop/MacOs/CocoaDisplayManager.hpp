#pragma once
#include "HodEngine/Window/Export.hpp"

#if defined(PLATFORM_MACOS)

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
    class HOD_WINDOW_API CocoaDisplayManager : public DesktopDisplayManager
    {
        _SingletonOverride(CocoaDisplayManager)

    public:

        bool            Initialize() override;
        void            Terminate() override;

        Window*         CreateWindow(bool hidden = false) override;
        void            DestroyWindow(Window* window) override;
    };
}

#endif
