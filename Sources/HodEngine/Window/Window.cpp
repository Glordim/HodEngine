#include "HodEngine/Window/Pch.hpp"
#include "Window.hpp"

#include "HodEngine/Window/Surface.hpp"

namespace hod::window
{
	/// @brief 
	Window::~Window()
	{
		DefaultAllocator::GetInstance().Delete(_surface);
	}

	/// @brief 
	void Window::Update()
	{

	}

	/// @brief 
	/// @return 
	Surface* Window::GetSurface() const
	{
		return _surface;
	}

	/// @brief 
	/// @param surface 
	void Window::SetSurface(Surface* surface)
	{
		_surface = surface;
	}

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
