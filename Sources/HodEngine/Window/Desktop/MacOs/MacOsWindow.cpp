#include "MacOsWindow.hpp"

#include <string>

#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Job/JobQueue.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <cstdlib>

namespace hod::window
{
	/// @brief 
	/// @return 
	NSView* MacOsWindow::GetNsView() const
	{
		return _view;
	}
}
