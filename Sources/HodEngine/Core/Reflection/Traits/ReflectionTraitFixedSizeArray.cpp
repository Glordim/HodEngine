#include "HodEngine/Core/Reflection/Traits/ReflectionTraitFixedSizeArray.h"

namespace hod
{
	namespace core
	{
		///@brief 
		ReflectionTraitFixedSizeArray::ReflectionTraitFixedSizeArray(uint32_t size)
		: ReflectionTrait()
		, _fixedSize(size)
		{
		}
		
		uint32_t ReflectionTraitFixedSizeArray::GetFixedSize() const
		{
			return _fixedSize;
		}
	}
}
