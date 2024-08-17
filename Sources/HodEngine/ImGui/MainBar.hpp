#pragma once
#include "HodEngine/ImGui/Export.hpp"

namespace hod::imgui
{
	/// @brief 
	class HOD_IMGUI_API MainBar
	{
	public:

							MainBar() = default;
		virtual				~MainBar() = default;

	public:

		virtual void		Draw() = 0;
	};
}
