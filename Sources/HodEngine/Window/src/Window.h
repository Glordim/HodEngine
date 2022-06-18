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

		uint32_t			GetWidth() const;
		uint32_t			GetHeight() const;

	private:

		renderer::Context*	_context = nullptr;

		uint32_t			_width = 0;
		uint32_t			_height = 0;
	};
}
