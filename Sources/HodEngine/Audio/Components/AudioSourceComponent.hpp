#pragma once
#include "HodEngine/Audio/AudioResource.hpp"
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include <HodEngine/Audio/AudioManager.hpp>

namespace hod::inline audio
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_AUDIO_API AudioSourceComponent : public Component
	{
		REFLECTED_CLASS(AudioSourceComponent, Component)

	public:

							AudioSourceComponent() = default;
							AudioSourceComponent(const AudioSourceComponent&) = delete;
							AudioSourceComponent(AudioSourceComponent&&) = delete;
							~AudioSourceComponent() override = default;

		void				operator=(const AudioSourceComponent&) = delete;
		void				operator=(AudioSourceComponent&&) = delete;

		void				OnStart() override;
		void				OnEnable() override;
		void				OnDisable() override;
		void				OnDestruct() override;

	private:

		WeakResource<AudioResource>	_audioResource;
		bool						_loop = true;

		AudioManager::PlayingId _playingId = AudioManager::InvalidPlayingId;
	};
}
