#include "HodEngine/ImGui/Window.h"

namespace hod::imgui
{
	bool Window::IsClosed() const
	{
		return _closed;
	}

	void Window::Close()
	{
		_closed = true;
	}
}
