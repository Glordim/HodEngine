#pragma once
#include <HodEngine/HodEngine.hpp>

#include <stdint.h>

namespace hod::window
{
	class Surface;

	/// @brief 
	class HOD_API Window
	{
	public:

		virtual				~Window();

	public:

		virtual void		Update();

		Surface*			GetSurface() const;
		void				SetSurface(Surface* surface);

		uint16_t			GetWidth() const;
		uint16_t			GetHeight() const;

	protected:

		uint16_t			_width = 800;
		uint16_t			_height = 600;

	private:

		Surface*			_surface = nullptr;
	};
}
