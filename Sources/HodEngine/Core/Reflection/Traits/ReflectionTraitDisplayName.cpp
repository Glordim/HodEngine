#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.h"

namespace hod
{
	namespace core
	{
		///@brief 
		ReflectionTraitDisplayName::ReflectionTraitDisplayName(const std::string_view& name)
		: ReflectionTrait()
		, _name(name)
		{
		}
		
		const std::string& ReflectionTraitDisplayName::GetValue() const
		{
			return _name;
		}
	}
}
