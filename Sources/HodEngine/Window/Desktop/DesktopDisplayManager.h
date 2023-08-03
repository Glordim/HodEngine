#pragma once

#include "HodEngine/Window/DisplayManager.h"

namespace hod::window
{
    class Window;

    class DesktopDisplayManager : public DisplayManager
    {
        //_SingletonOverride(DesktopDisplayManager)

    public:

        virtual Window* CreateWindow() = 0;
        virtual void    DestroyWindow(Window* window) = 0;
    };
}
