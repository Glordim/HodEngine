#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

namespace hod::game
{
	_SingletonConstructor(SerializedDataFactory)
	{

	}

	/// @brief 
	/// @param type 
	/// @return 
	ReflectionDescriptor* SerializedDataFactory::FindReflectionDescriptor(MetaType type) const
	{
		auto it = _metaTypeToDescriptors.find(type);
		if (it != _metaTypeToDescriptors.end())
		{
			return it->second;
		}
		return nullptr;
	}
}
