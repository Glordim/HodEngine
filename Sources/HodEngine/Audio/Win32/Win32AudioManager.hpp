#pragma once
#include "HodEngine/Audio/Export.hpp"
#include "HodEngine/Audio/AudioManager.hpp"

#include "HodEngine/Core/Singleton.hpp"

namespace hod::audio
{
    class HOD_AUDIO_API Win32AudioManager : public AudioManager
    {
        _SingletonOverride(Win32AudioManager)

    public:

        bool    Initialize() override;
        void    Terminate() override;
    };
}
