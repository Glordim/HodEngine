#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Audio/AudioInstance.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <cstdint>
#include <mutex>

namespace hod::inline audio
{
	_SingletonConstructor(AudioManager)
	: _updateJob(this, &AudioManager::Update)
	{}

	/// @brief
	void AudioManager::InsertUpdateJob()
	{
		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::Render);
	}

	/// @brief
	void AudioManager::RemoveUpdateJob()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::Render);
	}

	AudioManager::PlayingId AudioManager::Play(std::shared_ptr<AudioResource> audioResource, bool loop)
	{
		if (audioResource == nullptr)
		{
			OUTPUT_ERROR("AudioManager::Play: audioResource is null");
			return InvalidPlayingId;
		}
		PlayingId playingId = _nextPlayingId.fetch_add(1);
		AudioInstance* instance = DefaultAllocator::GetInstance().New<AudioInstance>(audioResource, loop);
		std::scoped_lock lock(_lock);
		_audioInstanceMap[playingId] = instance;
		return playingId;
	}

	void AudioManager::Pause(PlayingId playingId)
	{
		auto it = _audioInstanceMap.find(playingId);
		if (it != _audioInstanceMap.end())
		{
			it->second->Pause();
		}
	}

	void AudioManager::Resume(PlayingId playingId)
	{
		auto it = _audioInstanceMap.find(playingId);
		if (it != _audioInstanceMap.end())
		{
			it->second->Resume();
		}
	}

	void AudioManager::Stop(PlayingId playingId)
	{
		std::scoped_lock lock(_lock);
		auto it = _audioInstanceMap.find(playingId);
		if (it != _audioInstanceMap.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_audioInstanceMap.erase(it);
		}
	}

	void AudioManager::PlayOneShot(std::shared_ptr<AudioResource> audioResource)
	{
		if (audioResource == nullptr)
		{
			OUTPUT_ERROR("AudioManager::PlayOneShot: audioResource is null");
			return;
		}
		AudioInstance* instance = DefaultAllocator::GetInstance().New<AudioInstance>(audioResource, false);

		std::scoped_lock lock(_lock);
		_oneshotInstances.PushBack(instance);
	}

	void AudioManager::FillBuffer(float* buffer, uint32_t sampleRate, uint32_t channelCount, uint32_t framesCount)
	{
		std::memset(buffer, 0, framesCount * channelCount * sizeof(float));

		_lock.lock();
		for (auto it : _audioInstanceMap)
		{
			AudioInstance* audioInstance = it.second;
			audioInstance->FillBuffer(buffer, sampleRate, channelCount, framesCount);
		}

		for (uint32_t i = _oneshotInstances.Size(); i-- > 0; )
		{
			AudioInstance* audioInstance = _oneshotInstances[i];
			audioInstance->FillBuffer(buffer, sampleRate, channelCount, framesCount);
			if (audioInstance->IsFinished())
			{
				DefaultAllocator::GetInstance().Delete(audioInstance);
				_oneshotInstances.Erase(i);
			}
		}
		_lock.unlock();

		/* TEST
		// Frequency of the sine wave (440Hz, standard A)
		const double frequency = 440.0;
		// Amplitude of the wave
		const double amplitude = 0.1;
		// Track the phase of the sine wave
		static double phase = 0.0;
		const double phaseIncrement = (2.0 * 3.14f * frequency) / sampleRate;

		for (uint32_t i = 0; i < framesCount; ++i)
		{
			float sampleValue = static_cast<float>(amplitude * sin(phase));
			for (uint32_t channel = 0; channel < channelCount; ++channel)
			{
				buffer[i * channelCount + channel] = sampleValue;
			}
			phase += phaseIncrement;
			if (phase >= 2.0 * 3.14f)
			{
				phase -= 2.0 * 3.14f;
			}
		}
		*/

		for (uint32_t i = 0; i < framesCount * channelCount; ++i)
		{
			buffer[i] = Clamp(buffer[i], -1.0f, 1.0f);
		}
	}
}
