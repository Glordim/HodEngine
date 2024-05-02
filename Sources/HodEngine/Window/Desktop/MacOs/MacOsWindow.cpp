#if defined(PLATFORM_MACOS)

#include "MacOsWindow.hpp"

#include <string>

#include <HodEngine/Core/Output.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Renderer/RHI/Context.hpp>

#include <cstdlib>

#include <swift/bridging>

using namespace hod::renderer;

namespace hod::window
{
	/// @brief 
	MacOsWindow::MacOsWindow()
		: DesktopWindow()
		, _nativeWinwdow(hod_swift::MacOsNativeWindow::init())
	{
		swift::String test = "Hello Swift world!";
		hod_swift::printWelcomeMessage(test);

		_nativeWinwdow.Show();
	}

	/// @brief 
	MacOsWindow::~MacOsWindow()
	{
		
	}

	/// @brief 
	void MacOsWindow::Update()
	{
		
	}

	/// @brief 
	/// @param width 
	/// @param height 
	void MacOsWindow::SetSize(uint16_t width, uint16_t height)
	{
		_width = width;
		_height = height;
	}

	/// @brief 
	void MacOsWindow::CenterToScreen()
	{
		_nativeWinwdow.Center();
	}

	/// @brief 
	void MacOsWindow::Maximize()
	{
		_nativeWinwdow.Maximize();
	}
}

#endif
