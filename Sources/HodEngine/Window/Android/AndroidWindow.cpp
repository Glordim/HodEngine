#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Android/AndroidWindow.hpp"
#include "HodEngine/Window/Surface.hpp"

#include <string>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <cstdlib>

#include <android/native_window.h>

namespace hod::window
{
	/// @brief 
	/// @param nativeWindow 
	AndroidWindow::AndroidWindow(ANativeWindow* nativeWindow)
	: _nativeWindow(nativeWindow)
	{
		_width = static_cast<uint16_t>(ANativeWindow_getWidth(_nativeWindow));
		_height = static_cast<uint16_t>(ANativeWindow_getHeight(_nativeWindow));
	}

	/// @brief 
	/// @return 
	ANativeWindow* AndroidWindow::GetNativeWindow() const
	{
		return _nativeWindow;
	}
}
