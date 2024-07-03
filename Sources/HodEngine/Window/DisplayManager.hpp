#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Singleton.hpp"

namespace hod::window
{
    class HOD_API DisplayManager
    {
        _SingletonAbstract(DisplayManager)

    public:

        virtual         ~DisplayManager() = default;

        virtual bool    Initialize() = 0;
        virtual void    Terminate() = 0;
    };
}
