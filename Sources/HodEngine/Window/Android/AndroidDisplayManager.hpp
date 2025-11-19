#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

struct ANativeWindow;

namespace hod::window
{
    class Window;

    class HOD_WINDOW_API AndroidDisplayManager : public DisplayManager
    {
        _SingletonOverride(AndroidDisplayManager)

    public:

        bool    Initialize() override { return true; }
        void    Update() override {};
        void    Terminate() override {}

        Window* CreateMainWindow(ANativeWindow* nativeWindow);
    };
}
