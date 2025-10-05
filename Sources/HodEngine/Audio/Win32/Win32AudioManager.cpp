#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Win32/Win32AudioManager.hpp"

#include "Mmdeviceapi.h"

namespace hod::audio
{
	_SingletonOverrideConstructor(Win32AudioManager) {}

	/// @brief
	/// @return
	bool Win32AudioManager::Initialize()
	{
		if (CoInitializeEx(nullptr, COINIT_MULTITHREADED) == RPC_E_CHANGED_MODE)
		{
			return false;
		}

		if (CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&_deviceEnumerator)) < 0)
		{
			return false;
		}

		if (_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device) < 0)
		{
			return false;
		}

		if (_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &_audioClient) < 0)
		{
			return false;
		}

		if (_audioClient->GetMixFormat(&_waveFormat) < 0)
		{
			return false;
		}

		if (_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, _waveFormat, nullptr) < 0)
		{
			return false;
		}

		if (_audioClient->GetService(IID_PPV_ARGS(&_renderClient)) < 0)
		{
			return false;
		}

		if (_audioClient->Start() < 0)
		{
			return false;
		}

		InsertUpdateJob();

		return true;
	}

	/// @brief
	void Win32AudioManager::Terminate()
	{
		RemoveUpdateJob();

		_audioClient->Stop();
		CoTaskMemFree(_waveFormat);

		CoUninitialize();
	}

	/// @brief
	void Win32AudioManager::Update()
	{
		UINT32 bufferFrameCount;
		if (_audioClient->GetBufferSize(&bufferFrameCount) < 0)
		{
			return;
		}

		BYTE* pData;
		DWORD flags = 0;

		UINT32 padding;
		if (_audioClient->GetCurrentPadding(&padding) < 0)
		{
			return;
		}

		UINT32 framesAvailable = bufferFrameCount - padding;

		if (_renderClient->GetBuffer(framesAvailable, &pData) < 0)
		{
			return;
		}

		/* ----------- Test -------
		// Frequency of the sine wave (440Hz, standard A)
		const double frequency = 440.0;
		// Amplitude of the wave
		const double amplitude = 0.1;
		// Track the phase of the sine wave
		double phase = 0.0;
		const double phaseIncrement = (2.0 * 3.14f * frequency) / _waveFormat->nSamplesPerSec;

		float* pFloatData = reinterpret_cast<float*>(pData);
		for (UINT32 i = 0; i < framesAvailable; ++i)
		{
		    float sampleValue = static_cast<float>(amplitude * sin(phase));
		    for (int channel = 0; channel < _waveFormat->nChannels; ++channel)
		    {
		        pFloatData[i * _waveFormat->nChannels + channel] = sampleValue;
		    }
		    phase += phaseIncrement;
		    if (phase >= 2.0 * 3.14f)
		    {
		        phase -= 2.0 * 3.14f;
		    }
		}
		*/

		if (_renderClient->ReleaseBuffer(framesAvailable, flags) < 0)
		{
			return;
		}
	}
}
