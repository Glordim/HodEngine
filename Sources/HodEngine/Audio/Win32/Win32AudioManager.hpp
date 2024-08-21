#pragma once
#include "HodEngine/Audio/Export.hpp"
#include "HodEngine/Audio/AudioManager.hpp"

#include "HodEngine/Core/Singleton.hpp"

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <wrl/client.h>
#include <avrt.h>

namespace hod::audio
{
    class HOD_AUDIO_API Win32AudioManager : public AudioManager
    {
        _SingletonOverride(Win32AudioManager)

    public:

        bool    Initialize() override;
        void    Terminate() override;

        void    Update() override;

    private:

        Microsoft::WRL::ComPtr<IMMDeviceEnumerator> _deviceEnumerator;
        Microsoft::WRL::ComPtr<IMMDevice> _device;
        Microsoft::WRL::ComPtr<IAudioClient> _audioClient;
        Microsoft::WRL::ComPtr<IAudioRenderClient> _renderClient;
        WAVEFORMATEX* _waveFormat;
    };
}
