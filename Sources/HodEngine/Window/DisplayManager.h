#pragma once

#include "HodEngine/Core/Singleton.h"

namespace hod::window
{
    class DisplayManager
    {
        _SingletonAbstract(DisplayManager)

    public:

        virtual         ~DisplayManager() = default;

        virtual bool    Initialize() = 0;
        virtual void    Terminate() = 0;
    };
}
