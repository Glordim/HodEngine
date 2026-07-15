#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataStructFactory.hpp"

namespace hod::inline game
{
	_SingletonConstructor(DataStructFactory)
	{

	}

	/// @brief
	/// @param type
	/// @return
	ReflectionDescriptor* DataStructFactory::FindReflectionDescriptor(RttiType type) const
	{
		auto it = _RttiTypeToDescriptors.find(type);
		if (it != _RttiTypeToDescriptors.end())
		{
			return it->second;
		}
		return nullptr;
	}
}
