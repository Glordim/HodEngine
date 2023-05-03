#include "HodEngine/Editor/RecentProjects.h"

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects)
	{
		core::Reflection::Property::Variable* projectsPath = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::Float32, offsetof(RecentProjects, _projectsPath), "ProjectsPath");
		AddProperty(projectsPath);
	}
}
