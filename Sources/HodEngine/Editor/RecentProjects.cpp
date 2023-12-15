#include "HodEngine/Editor/RecentProjects.h"

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects, void)
	{
		//ADD_PROPERTY(RecentProjects, _projectsPath);
		ReflectionPropertyArray* projectsPath = new ReflectionPropertyArray(ReflectionPropertyVariable::Type::String, offsetof(RecentProjects, _projectsPath), "ProjectsPath");
		AddProperty(projectsPath);
	}
}
