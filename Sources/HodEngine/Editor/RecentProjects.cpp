#include "HodEngine/Editor/RecentProjects.h"

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects)
	{
		core::Reflection::Property::Array* projectsPath = new core::Reflection::Property::Array(core::Reflection::Property::Variable::String, offsetof(RecentProjects, _projectsPath), "ProjectsPath");
		AddProperty(projectsPath);
	}
}
