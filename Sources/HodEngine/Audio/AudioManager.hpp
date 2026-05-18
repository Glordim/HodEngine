#pragma once
#include "HodEngine/Audio/AudioResource.hpp"
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/GameSystems/Job/MemberFunctionJob.hpp"
#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include <cstdint>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace hod::inline audio
{
	class AudioInstance;

	class HOD_AUDIO_API AudioManager
	{
		_SingletonAbstract(AudioManager)

	public:

		using PlayingId = uint64_t;
		static constexpr PlayingId InvalidPlayingId = -1;

	public:
		virtual ~AudioManager() = default;

		virtual bool Initialize() = 0;
		virtual void Terminate() = 0;

		virtual void Update() = 0;

		[[nodiscard]] PlayingId	Play(std::shared_ptr<AudioResource> audioResource, bool loop);
		void		Pause(PlayingId playingId);
		void		Resume(PlayingId playingId);
		void		Stop(PlayingId playingId);

		void		PlayOneShot(std::shared_ptr<AudioResource> audioResource);

	protected:
		void InsertUpdateJob();
		void RemoveUpdateJob();

		void FillBuffer(float* buffer, uint32_t sampleRate, uint32_t channelCount, uint32_t framesCount);

	private:
		MemberFunctionJob<AudioManager> _updateJob;

		std::atomic<PlayingId> _nextPlayingId = { 0 };

		std::unordered_map<PlayingId, AudioInstance*> _audioInstanceMap;
		Vector<AudioInstance*> _oneshotInstances;

		std::mutex _lock;
	};
}
