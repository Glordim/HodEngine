#pragma once
#include <HodEngine/HodEngine.hpp>

#include <stdint.h>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include <vector>
#include <string>

namespace hod::editor
{
	struct HOD_API RecentProjects
	{
		REFLECTED_CLASS_NO_VIRTUAL(RecentProjects)

	public:

		std::vector<std::string>	_projectsPath;
	};
}
