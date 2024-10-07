#pragma once

#if defined(PLATFORM_WINDOWS)

	#include "HodEngine/Audio/Win32/Win32AudioManager.hpp"
	#define PlatformAudioManager hod::audio::Win32AudioManager

#elif defined(PLATFORM_MACOS)

	#include "HodEngine/Audio/MacOs/MacOsAudioManager.hpp"
	#define PlatformAudioManager hod::audio::MacOsAudioManager

#elif defined(PLATFORM_LINUX)

	//#include "HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp"
	//#define PlatformDisplayManager hod::window::WaylandDisplayManager

#elif defined(PLATFORM_ANDROID)

	#include "HodEngine/Audio/Android/AndroidAudioManager.hpp"
	#define PlatformAudioManager hod::audio::AndroidAudioManager

#elif defined(PLATFORM_IOS)

	//#include "GraphicApplications/MobileApplications/IOSApplication.hpp"
	//#define PlatformApplication IOSApplication

#endif
