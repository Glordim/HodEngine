#include "HodEngine/ImGui/Window.h" 

namespace hod::imgui
{
	Window::Window()
	{
		
	}

	bool Window::IsClosed() const
	{
		return _closed;
	}

	void Window::Close()
	{
		_closed = true;
	}

	const char* Window::GetIdentifier()
	{
		Type type = GetType();
		_identifier = std::to_string(type);

		return _identifier.c_str();
	}
}
