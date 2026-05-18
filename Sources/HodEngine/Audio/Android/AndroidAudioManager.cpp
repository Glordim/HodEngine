#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Android/AndroidAudioManager.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include <aaudio/AAudio.h>

namespace hod::inline audio
{
	_SingletonOverrideConstructor(AndroidAudioManager) {}

	/// @brief
	int32_t AndroidAudioManager::DataCallback(AAudioStream* /*stream*/, void* userData, void* audioData, int32_t numFrames)
	{
		AndroidAudioManager* self = reinterpret_cast<AndroidAudioManager*>(userData);
		self->FillBuffer(reinterpret_cast<float*>(audioData), self->_sampleRate, self->_channelCount, static_cast<uint32_t>(numFrames));
		return AAUDIO_CALLBACK_RESULT_CONTINUE;
	}

	/// @brief
	/// @return
	bool AndroidAudioManager::Initialize()
	{
		AAudioStreamBuilder* builder = nullptr;
		if (AAudio_createStreamBuilder(&builder) != AAUDIO_OK)
		{
			OUTPUT_ERROR("AndroidAudioManager::Initialize: AAudio_createStreamBuilder fail");
			return false;
		}

		AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
		AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
		AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
		AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
		AAudioStreamBuilder_setDataCallback(builder, DataCallback, this);

		aaudio_result_t result = AAudioStreamBuilder_openStream(builder, &_stream);
		AAudioStreamBuilder_delete(builder);

		if (result != AAUDIO_OK)
		{
			OUTPUT_ERROR("AndroidAudioManager::Initialize: AAudioStreamBuilder_openStream fail");
			return false;
		}

		_sampleRate = AAudioStream_getSampleRate(_stream);
		_channelCount = AAudioStream_getChannelCount(_stream);

		if (AAudioStream_requestStart(_stream) != AAUDIO_OK)
		{
			OUTPUT_ERROR("AndroidAudioManager::Initialize: AAudioStream_requestStart fail");
			AAudioStream_close(_stream);
			_stream = nullptr;
			return false;
		}

		return true;
	}

	/// @brief
	void AndroidAudioManager::Terminate()
	{
		if (_stream != nullptr)
		{
			AAudioStream_requestStop(_stream);
			AAudioStream_close(_stream);
			_stream = nullptr;
		}
	}

	/// @brief
	void AndroidAudioManager::Update()
	{
		// Audio driven by AAudio callback
	}
}
