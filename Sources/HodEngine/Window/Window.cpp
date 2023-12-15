#include "Window.hpp"

#include "HodEngine/Renderer/RHI/Context.hpp"

namespace hod::window
{
	/// @brief 
	Window::~Window()
	{
		delete _graphicsContext;
	}

	/// @brief 
	void Window::Update()
	{

	}

	/// @brief 
	/// @return 
	renderer::Context* Window::GetGraphicsContext() const
	{
		return _graphicsContext;
	}

	/// @brief 
	/// @param context 
	void Window::SetGraphicsContext(renderer::Context* graphicsContext)
	{
		_graphicsContext = graphicsContext;
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
