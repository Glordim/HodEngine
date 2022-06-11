#pragma once

#include <stdint.h>

namespace HOD
{
	namespace RENDERER
	{
		class Context;
	}

	/// @brief 
	class Window
	{
	public:

		virtual				~Window();

	public:

		virtual void		Update();

		RENDERER::Context*	GetContext() const;
		void				SetContext(RENDERER::Context* context);

		uint32_t			GetWidth() const;
		uint32_t			GetHeight() const;

	private:

		RENDERER::Context*	_context = nullptr;

		uint32_t			_width = 0;
		uint32_t			_height = 0;
	};
}
