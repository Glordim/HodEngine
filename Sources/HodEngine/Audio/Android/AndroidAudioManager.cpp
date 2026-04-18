#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Android/AndroidAudioManager.hpp"

namespace hod::inline audio
{
	_SingletonOverrideConstructor(AndroidAudioManager) {}

	/// @brief
	/// @return
	bool AndroidAudioManager::Initialize()
	{
		return true;
	}

	/// @brief
	void AndroidAudioManager::Terminate() {}

	/// @brief
	void AndroidAudioManager::Update() {}
}
