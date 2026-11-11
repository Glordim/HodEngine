#include "HodEngine/AudioComponents/Pch.hpp"
#include "HodEngine/Audio/AudioManager.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/AudioComponents/Components/AudioSourceComponent.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline audio
{
	DESCRIBE_REFLECTED_CLASS(AudioSourceComponent, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_VOLUME_HIGH);
		reflectionDescriptor.AddTrait<ReflectionTraitCategory>("Audio");
		reflectionDescriptor.AddTrait<ReflectionTraitDescription>("Plays an audio clip, once or in a loop");
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
