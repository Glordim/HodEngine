#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	_SingletonConstructor(ComponentFactory)
	{

	}

	/// @brief
	/// @param descriptor
	/// @return
	bool ComponentFactory::Register(ReflectionDescriptor& descriptor)
	{
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("ComponentFactory: the component '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
		return true;
	}

	/// @brief
	/// @param descriptor
	/// @return
	bool ComponentFactory::Unregister(const ReflectionDescriptor& descriptor)
	{
		return _typeToDescriptors.erase(descriptor.GetType()) != 0;
	}
}
