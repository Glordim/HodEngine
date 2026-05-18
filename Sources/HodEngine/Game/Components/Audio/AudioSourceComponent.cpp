#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Game/Components/Audio/AudioSourceComponent.hpp"

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(AudioSourceComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AudioSourceComponent::_audioResource, "Audio");
		AddPropertyT(reflectionDescriptor, &AudioSourceComponent::_loop, "Loop");
	}

	void AudioSourceComponent::OnStart()
	{
		_playingId = AudioManager::GetInstance()->Play(_audioResource.Lock(), _loop);
	}

	void AudioSourceComponent::OnEnable()
	{
		if (_playingId != AudioManager::InvalidPlayingId)
		{
			AudioManager::GetInstance()->Resume(_playingId);
		}
	}

	void AudioSourceComponent::OnDisable()
	{
		if (_playingId != AudioManager::InvalidPlayingId)
		{
			AudioManager::GetInstance()->Pause(_playingId);
		}
	}

	void AudioSourceComponent::OnDestruct()
	{
		if (_playingId != AudioManager::InvalidPlayingId)
		{
			AudioManager::GetInstance()->Stop(_playingId);
		}
	}
}
