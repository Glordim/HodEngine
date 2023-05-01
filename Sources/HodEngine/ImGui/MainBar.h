#pragma once

namespace hod::imgui
{
	/// @brief 
	class MainBar
	{
	public:

							MainBar() = default;
		virtual				~MainBar() = default;

	public:

		virtual void		Draw() = 0;
	};
}
