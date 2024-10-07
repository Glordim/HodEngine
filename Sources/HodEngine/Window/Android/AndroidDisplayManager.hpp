#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/DisplayManager.hpp"

#undef CreateWindow

namespace hod::window
{
    class Window;

    class HOD_WINDOW_API AndroidDisplayManager : public DisplayManager
    {
        _SingletonOverride(AndroidDisplayManager)

    public:

        bool    Initialize() override { return false; }
        void    Terminate() override {}
    };
}
