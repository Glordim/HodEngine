#pragma once
#include "HodEngine/UI/Export.hpp"

namespace hod::ui
{
	/// @brief 
	class IRebuildable
	{
	public:

		virtual bool	Rebuild() = 0;
	};
}
