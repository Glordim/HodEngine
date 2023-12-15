#include "ReflectionTraitGetValueForSerialization.hpp"

namespace hod
{
	/// @brief 
	/// @tparam _ReturnType_ 
	/// @tparam _InstanceType_ 
	/// @param method 
	ReflectionTraitGetValueForSerialization::ReflectionTraitGetValueForSerialization(const std::function<void(const void*, void*)>& method, uint32_t size)
	: ReflectionTrait()
	, _method(method)
	, _size(size)
	{
	}

	/// @brief 
	/// @tparam _ReturnType_ 
	/// @tparam _InstanceType_ 
	/// @param instance 
	/// @return 
	void ReflectionTraitGetValueForSerialization::GetValueForSerialization(const void* instance, void* value) const
	{
		_method(instance, value);
	}
}
