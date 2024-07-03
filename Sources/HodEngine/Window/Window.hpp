#pragma once
#include <HodEngine/HodEngine.hpp>

#include <stdint.h>

namespace hod::renderer
{
	class Context;
}

namespace hod::window
{
	/// @brief 
	class HOD_API Window
	{
	public:

		virtual				~Window();

	public:

		virtual void		Update();

		renderer::Context*	GetGraphicsContext() const;
		void				SetGraphicsContext(renderer::Context* graphicsContext);

		uint16_t			GetWidth() const;
		uint16_t			GetHeight() const;

	protected:

		uint16_t			_width = 800;
		uint16_t			_height = 600;

	private:

		renderer::Context*	_graphicsContext = nullptr;
	};
}
