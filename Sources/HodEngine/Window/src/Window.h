#pragma once

#include <stdint.h>

namespace hod::renderer
{
	class Context;
}

namespace hod::window
{
	/// @brief 
	class Window
	{
	public:

		virtual				~Window();

	public:

		virtual void		Update();

		renderer::Context*	GetContext() const;
		void				SetContext(renderer::Context* context);

		uint16_t			GetWidth() const;
		uint16_t			GetHeight() const;

	protected:

		uint16_t			_width = 0;
		uint16_t			_height = 0;

	private:

		renderer::Context*	_context = nullptr;
	};
}
