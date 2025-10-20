#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod::editor
{
	struct HOD_EDITOR_API RecentProjects
	{
		REFLECTED_CLASS_NO_VIRTUAL(RecentProjects)

	public:
		Vector<String> _projectsPath;
	};
}
