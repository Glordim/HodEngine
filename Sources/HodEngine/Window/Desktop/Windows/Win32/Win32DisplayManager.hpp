#pragma once
#include "HodEngine/Window/Export.hpp"

#if defined(PLATFORM_WINDOWS)

#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/Window/Desktop/DesktopDisplayManager.hpp"

namespace hod::window
{
    class HOD_WINDOW_API Win32DisplayManager : public DesktopDisplayManager
    {
        _SingletonOverride(Win32DisplayManager)

    public:

        bool            Initialize() override;
        void            Update() override;
        void            Terminate() override;

        Window*         CreateWindow(bool hidden = false) override;
        void            DestroyWindow(Window* window) override;

    private:

        std::vector<Window*> _windows;
    };
}

#endif
