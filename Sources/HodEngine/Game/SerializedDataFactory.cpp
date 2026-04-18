#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

namespace hod::inline game
{
	_SingletonConstructor(SerializedDataFactory)
	{

	}

	/// @brief 
	/// @param type 
	/// @return 
	ReflectionDescriptor* SerializedDataFactory::FindReflectionDescriptor(RttiType type) const
	{
		auto it = _RttiTypeToDescriptors.find(type);
		if (it != _RttiTypeToDescriptors.end())
		{
			return it->second;
		}
		return nullptr;
	}
}
