#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/Linux/LinuxAudioManager.hpp"

namespace hod::audio
{
	_SingletonOverrideConstructor(LinuxAudioManager)
	{

	}

	/// @brief 
	/// @return 
	bool LinuxAudioManager::Initialize()
	{
		return true;
	}

	/// @brief 
	void LinuxAudioManager::Terminate()
	{
	}

	/// @brief 
	void LinuxAudioManager::Update()
	{
	}
}
