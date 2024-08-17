#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"

namespace hod::window
{
    class HOD_WINDOW_API DisplayManager
    {
        _SingletonAbstract(DisplayManager)

    public:

        virtual         ~DisplayManager() = default;

        virtual bool    Initialize() = 0;
        virtual void    Terminate() = 0;
    };
}
