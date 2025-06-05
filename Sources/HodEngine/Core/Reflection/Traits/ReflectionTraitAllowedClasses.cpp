#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitAllowedClasses.hpp"

namespace hod
{
	/// @brief 
	/// @return 
	const Vector<ReflectionDescriptor*>& ReflectionTraitAllowedClasses::GetAllowedClasses() const
	{
		return _allowedClasses;
	}

	/// @brief 
	/// @param reflectionDescriptor 
	void ReflectionTraitAllowedClasses::AddAllowedClass(ReflectionDescriptor* reflectionDescriptor)
	{
		_allowedClasses.push_back(reflectionDescriptor);
	}
}
