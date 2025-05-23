#pragma once
#include "HodEngine/Core/Export.hpp"

#include <string_view>

namespace hod
{
	class HOD_CORE_API Process
	{
	public:
		static bool Create(const std::string_view& program, const std::string_view& argument, bool detach); // TODO argument should be pass as array because split string by space can split a textual argument with a space inside ?
	};
}
