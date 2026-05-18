#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/AudioInstance.hpp"

namespace hod::inline audio
{
	/// @brief
	/// @return
	AudioInstance::AudioInstance(std::shared_ptr<AudioResource> audioResource, bool loop)
	: _audioResource(audioResource)
	, _loop(loop)
	{
	}

	void AudioInstance::Pause()
	{
		_paused = true;
	}

	void AudioInstance::Resume()
	{
		_paused = false;
	}

	void AudioInstance::FillBuffer(float* buffer, uint32_t sampleRate, uint32_t channelCount, uint32_t framesCount)
	{
		if (_paused || _finished || _audioResource == nullptr)
			return;

		uint32_t srcChannels = _audioResource->_channelCount;
		float srcRatio = static_cast<float>(_audioResource->_sampleRate) / static_cast<float>(sampleRate);
		uint32_t startFrame = static_cast<uint32_t>(_time * _audioResource->_sampleRate);

		if (_loop && _audioResource->_frameCount > 0)
			startFrame %= _audioResource->_frameCount;

		for (uint32_t i = 0; i < framesCount; ++i)
		{
			uint32_t srcFrame = startFrame + static_cast<uint32_t>(i * _speed * srcRatio);

			if (srcFrame >= _audioResource->_frameCount)
			{
				if (_loop)
					srcFrame %= _audioResource->_frameCount;
				else
				{
					_finished = true;
					break;
				}
			}

			for (uint32_t ch = 0; ch < channelCount; ++ch)
			{
				uint32_t srcCh = (ch < srcChannels) ? ch : (srcChannels - 1);
				buffer[i * channelCount + ch] += _audioResource->_samples[srcFrame * srcChannels + srcCh];
			}
		}

		_time += static_cast<float>(framesCount) * _speed / static_cast<float>(sampleRate);

		if (_loop && _audioResource->_frameCount > 0)
		{
			float duration = static_cast<float>(_audioResource->_frameCount) / static_cast<float>(_audioResource->_sampleRate);
			while (_time >= duration)
				_time -= duration;
		}
	}
}
