#pragma once

#include "HodEngine/Window/Window.h"

namespace hod::window
{
	/// @brief 
	class DesktopWindow : public Window
	{
	public:

		virtual void			SetSize(uint16_t width, uint16_t height) = 0;
		virtual void			CenterToScreen() = 0;
		virtual void			Maximize() = 0;
	};
}
