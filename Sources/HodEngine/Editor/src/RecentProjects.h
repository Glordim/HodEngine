#pragma once

#include <stdint.h>

#include "HodEngine/Core/Src/Reflection/ReflectionMacros.h"

#include <vector>
#include <string>

namespace hod::editor
{
	struct RecentProjects
	{
		REFLECTED_CLASS(RecentProjects)

	public:

		std::vector<std::string>	_projectsPath;
	};
}
