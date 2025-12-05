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

	void Window::SetSizeInternal(uint16_t width, uint16_t height)
	{
		if (width != _width || height != _height)
		{
			_width = width;
			_height = height;

			_onResizeEvent.Emit(_width, _height);
		}
	}

	/// @brief
	/// @return
	bool Window::IsClose() const
	{
		return _close;
	}

	Event<uint16_t, uint16_t>& Window::OnResizeEvent()
	{
		return _onResizeEvent;
	}
}
