#include "HodEngine/Editor/RecentProjects.h"

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects, void)
	{
		//ADD_PROPERTY(RecentProjects, _projectsPath);
		Reflection::Property::Array* projectsPath = new Reflection::Property::Array(Reflection::Property::Variable::Type::String, offsetof(RecentProjects, _projectsPath), "ProjectsPath");
		AddProperty(projectsPath);
	}
}
