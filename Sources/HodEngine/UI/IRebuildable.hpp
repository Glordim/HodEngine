#pragma once
#include "HodEngine/UI/Export.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API IRebuildable
	{
	public:

		virtual bool	Rebuild() = 0;
	};
}
