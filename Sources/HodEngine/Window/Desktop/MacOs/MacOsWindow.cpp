#if defined(PLATFORM_MACOS)

#include "MacOsWindow.hpp"

#include <string>

#include <HodEngine/Core/Output.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Renderer/RHI/Context.hpp>

#include <cstdlib>

using namespace hod::renderer;

namespace hod::window
{
	/// @brief 
	MacOsWindow::MacOsWindow()
		: DesktopWindow()
	{
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
		//_nativeWinwdow.Center();
	}

	/// @brief 
	void MacOsWindow::Maximize()
	{
		//_nativeWinwdow.Maximize();
	}

	/// @brief 
	/// @return 
	NSView* MacOsWindow::GetNsView() const
	{
		return _view;
	}
}

#endif
