#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

#undef CreateWindow

namespace hod::window
{
    class Window;

    class HOD_WINDOW_API DesktopDisplayManager : public DisplayManager
    {
        _SingletonAbstract(DesktopDisplayManager)

    public:

        virtual Window* CreateWindow(bool hidden = false) = 0;
        virtual void    DestroyWindow(Window* window) = 0;
    };
}
