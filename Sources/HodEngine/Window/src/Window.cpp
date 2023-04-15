#include "Window.h"

namespace hod::window
{
	/// @brief 
	Window::~Window()
	{
		delete _context;
	}

	/// @brief 
	void Window::Update()
	{

	}

	/// @brief 
	/// @return 
	renderer::Context* Window::GetContext() const
	{
		return _context;
	}

	/// @brief 
	/// @param context 
	void Window::SetContext(renderer::Context* context)
	{
		_context = context;
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
}
