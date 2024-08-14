#if defined(PLATFORM_MACOS)

#include "MacOsWindow.hpp"

#include <string>

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <cstdlib>

using namespace hod::renderer;

namespace hod::window
{
	/// @brief 
	/// @return 
	NSView* MacOsWindow::GetNsView() const
	{
		return _view;
	}

	/// @brief 
	void MacOsWindow::Update()
	{
		static bool once = false;
		if (once == false)
		{
			SetupLayer();
			once = true;
		}

		EventLoop();
	}
}

#endif
