#include "HodEngine/Window/Pch.hpp"
#include "Window.hpp"

namespace hod::window
{
	DESCRIBE_REFLECTED_CLASS(Window, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	Window::~Window() {}

	/// @brief
	void Window::Update() {}

	/// @brief
	/// @return
	uint16_t Window::GetWidth() const
	{
		return _width;
	}

	/// @brief
	/// @return
	uint16_t Window::GetHeight() const
	{
		return _height;
	}

	/// @brief
	/// @return
	bool Window::IsClose() const
	{
		return _close;
	}
}
