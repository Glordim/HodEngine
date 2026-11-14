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
	DESCRIBE_REFLECTED_CLASS(AudioSourceComponent)
	{
		HOD_TRAIT(ReflectionTraitIcon, ICON_MDI_VOLUME_HIGH);
		HOD_TRAIT(ReflectionTraitCategory, "Audio");
		HOD_TRAIT(ReflectionTraitDescription, "Plays an audio clip, once or in a loop");

		HOD_PROPERTY_NAMED(_audioResource, "Audio");
		HOD_PROPERTY(_loop);
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
