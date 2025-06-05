#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/String.hpp"

namespace hod::editor
{
	struct HOD_EDITOR_API RecentProjects
	{
		REFLECTED_CLASS_NO_VIRTUAL(RecentProjects)

	public:

		Vector<std::string>	_projectsPath;
	};
}
