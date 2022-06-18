#include "ReflectionProperty.h"

namespace hod
{
	namespace CORE
	{
		///@brief Construct a new ReflectionProperty::ReflectionProperty object
		///@param typeName 
		ReflectionProperty::ReflectionProperty(const char* typeName)
		: _typeName(typeName)
		{

		}

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
