#include "RecentProjects.h"

#include "HodEngine/Core/Src/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Src/Reflection/Properties/ReflectionPropertyVariable.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects)
	{
		auto descriptor = RecentProjects::GetReflectionDescriptor();

		core::Reflection::Property::Variable* projectsPath = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::Float32, offsetof(RecentProjects, _projectsPath), "ProjectsPath");
		descriptor->AddProperty(projectsPath);
	}
}
