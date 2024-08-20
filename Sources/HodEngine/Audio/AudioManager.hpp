#pragma once
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"

namespace hod::audio
{
    class HOD_AUDIO_API AudioManager
    {
        _SingletonAbstract(AudioManager)

    public:

        virtual         ~AudioManager() = default;

        virtual bool    Initialize() = 0;
        virtual void    Terminate() = 0;
    };
}
