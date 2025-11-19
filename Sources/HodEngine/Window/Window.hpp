#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <stdint.h>

namespace hod::window
{
	class Surface;

	/// @brief 
	class HOD_WINDOW_API Window
	{
		REFLECTED_CLASS_NO_PARENT(Window)

	public:

		virtual				~Window();

	public:

		virtual void		Update();

		Surface*			GetSurface() const;
		void				SetSurface(Surface* surface);

		uint16_t			GetWidth() const;
		uint16_t			GetHeight() const;

		bool				IsClose() const;

	protected:

		uint16_t			_width = 800;
		uint16_t			_height = 600;
		bool				_close = false;

	private:

		Surface*			_surface = nullptr;
	};
}
