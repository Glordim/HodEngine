#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Win32/Win32AudioManager.hpp"

#include "Mmdeviceapi.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"

namespace hod::inline audio
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

		if (_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 500000, 0, _waveFormat, nullptr) < 0)
		{
			return false;
		}

		if (_audioClient->GetService(IID_PPV_ARGS(&_renderClient)) < 0)
		{
			return false;
		}

		if (_audioClient->GetBufferSize(&_bufferFrameCount) < 0)
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
		BYTE* pData;
		DWORD flags = 0;

		UINT32 padding;
		if (_audioClient->GetCurrentPadding(&padding) < 0)
		{
			return;
		}

		UINT32 framesAvailable = _bufferFrameCount - padding;
		if (framesAvailable > 0)
		{
			if (_renderClient->GetBuffer(framesAvailable, &pData) < 0)
			{
				return;
			}

			FillBuffer(reinterpret_cast<float*>(pData), _waveFormat->nSamplesPerSec, _waveFormat->nChannels, framesAvailable);

			if (_renderClient->ReleaseBuffer(framesAvailable, flags) < 0)
			{
				return;
			}
		}
	}
}

#pragma clang diagnostic pop
