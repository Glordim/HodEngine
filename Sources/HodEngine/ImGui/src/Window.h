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

		bool				IsClosed() const;
		void				Close();

	private:

		bool				_closed = false;
	};
}
