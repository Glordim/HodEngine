#pragma once
#include "HodEngine/Window/Export.hpp"
#include "HodEngine/Window/Window.hpp"

struct ANativeWindow;

namespace hod::window
{
	/// @brief 
	class HOD_WINDOW_API AndroidWindow : public Window
	{
	public:
								AndroidWindow(ANativeWindow* nativeWindow);

		ANativeWindow*			GetNativeWindow() const;

	private:

		ANativeWindow*			_nativeWindow = nullptr;
	};
}
