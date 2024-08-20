#include "HodEngine/Audio/Win32/Win32AudioManager.hpp"

#include "Mmdeviceapi.h"

namespace hod::audio
{
    _SingletonOverrideConstructor(Win32AudioManager)
	{

	}

	/// @brief 
	/// @return 
	bool Win32AudioManager::Initialize()
	{
		return false;
	}

    /// @brief 
    void Win32AudioManager::Terminate()
	{
		
	}
}
