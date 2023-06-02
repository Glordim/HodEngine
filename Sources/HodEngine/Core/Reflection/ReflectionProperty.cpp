#include "HodEngine/Core/Reflection/ReflectionProperty.h"

namespace hod
{
	namespace core
	{
		///@brief 
		///@return const std::vector<ReflectionTrait>& 
		const std::vector<ReflectionTrait*>& ReflectionProperty::GetTraits() const
		{
			return _traits;
		}

		///@brief 
		///@param trait 
		void ReflectionProperty::AddTrait(ReflectionTrait* trait)
		{
			_traits.push_back(trait);
		}
	}
}
