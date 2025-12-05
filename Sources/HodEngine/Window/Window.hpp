#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <HodEngine/Core/Event.hpp>

#include <stdint.h>

namespace hod::window
{
	class Surface;

	/// @brief
	class HOD_WINDOW_API Window
	{
		REFLECTED_CLASS_NO_PARENT(Window)

	public:
		virtual ~Window();

	public:
		virtual void Update();

		uint16_t GetWidth() const;
		uint16_t GetHeight() const;

		bool IsClose() const;

		Event<uint16_t, uint16_t>& OnResizeEvent();

	protected:
		void SetSizeInternal(uint16_t width, uint16_t height);

	protected:
		uint16_t _width = 800;
		uint16_t _height = 600;
		bool     _close = false;

		Event<uint16_t, uint16_t> _onResizeEvent;
	};
}
