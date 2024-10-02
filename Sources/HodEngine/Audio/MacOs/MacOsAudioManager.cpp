#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/MacOs/MacOsAudioManager.hpp"

namespace hod::audio
{
	_SingletonOverrideConstructor(MacOsAudioManager)
	{

	}

	/// @brief 
	/// @return 
	bool MacOsAudioManager::Initialize()
	{
		return true;
	}

	/// @brief 
	void MacOsAudioManager::Terminate()
	{
	}

	/// @brief 
	void MacOsAudioManager::Update()
	{
	}
}
