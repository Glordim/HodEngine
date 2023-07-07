#include "HodEngine/Core/Reflection/Traits/ReflectionTraitFixedSizeArray.h"

namespace hod
{
	///@brief 
	ReflectionTraitFixedSizeArray::ReflectionTraitFixedSizeArray(uint32_t size)
	: ReflectionTrait()
	, _fixedSize(size)
	{
	}
	
	/// @brief 
	/// @return 
	uint32_t ReflectionTraitFixedSizeArray::GetFixedSize() const
	{
		return _fixedSize;
	}
}
