#include "HodEngine/ImGui/Window.hpp" 

namespace hod::imgui
{
	std::vector<WindowDescription*>	WindowDescription::_descriptions;

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
		_identifier = GetDescription()->GetName();
		return _identifier.c_str();
	}
}
