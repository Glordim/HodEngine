#pragma once

#include "HodEngine/Window/DisplayManager.hpp"

#undef CreateWindow

namespace hod::window
{
    class Window;

    class DesktopDisplayManager : public DisplayManager
    {
        _SingletonAbstract(DesktopDisplayManager)

    public:

        virtual Window* CreateWindow() = 0;
        virtual void    DestroyWindow(Window* window) = 0;
    };
}
