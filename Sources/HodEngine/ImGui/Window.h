#pragma once

#include "HodEngine/Core/Type.h"

#include <string>

namespace hod::imgui
{
	/// @brief 
	class Window
	{
		BASE_META_TYPE(Window);

	public:

							Window();
		virtual				~Window() = default;

	public:

		virtual void		Draw() = 0;

		bool				IsClosed() const;
		void				Close();

		const char*			GetIdentifier();

	private:

		bool				_closed = false;

		std::string			_identifier;
	};
}
