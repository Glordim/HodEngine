#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include <vector>
#include <string>

namespace hod::editor
{
	struct HOD_EDITOR_API RecentProjects
	{
		REFLECTED_CLASS_NO_VIRTUAL(RecentProjects, HOD_EDITOR_API)

	public:

		std::vector<std::string>	_projectsPath;
	};
}
