#include "Window.h"

namespace HOD
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
	RENDERER::Context* Window::GetContext() const
	{
		return _context;
	}

	/// @brief 
	/// @param context 
	void Window::SetContext(RENDERER::Context* context)
	{
		_context = context;
	}

	/// @brief 
	/// @return 
	uint32_t Window::GetWidth() const
	{
		return _width;
	}

	/// @brief 
	/// @return 
	uint32_t Window::GetHeight() const
	{
		return _height;
	}
}
