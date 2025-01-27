#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/RecentProjects.hpp"

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(RecentProjects, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &RecentProjects::_projectsPath, "_projectsPath");
	}
}
