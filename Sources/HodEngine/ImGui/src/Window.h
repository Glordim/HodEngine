#pragma once

namespace hod::imgui
{
	/// @brief 
	class Window
	{
	public:

							Window() = default;
		virtual				~Window() = default;

	public:

		virtual void		Draw() = 0;
	};
}
