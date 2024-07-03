#pragma once
#include <HodEngine/HodEngine.hpp>

namespace hod::imgui
{
	/// @brief 
	class HOD_API MainBar
	{
	public:

							MainBar() = default;
		virtual				~MainBar() = default;

	public:

		virtual void		Draw() = 0;
	};
}
