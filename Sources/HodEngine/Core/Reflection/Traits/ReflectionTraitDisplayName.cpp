#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"

namespace hod
{
	///@brief 
	ReflectionTraitDisplayName::ReflectionTraitDisplayName(const std::string_view& name)
	: ReflectionTrait()
	, _name(name)
	{
	}
	
	/// @brief 
	/// @return 
	const String& ReflectionTraitDisplayName::GetValue() const
	{
		return _name;
	}
}
