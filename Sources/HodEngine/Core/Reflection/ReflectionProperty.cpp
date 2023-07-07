#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/ReflectionTrait.h"

#include <algorithm>

namespace hod
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

	/// @brief 
	/// @param metaType 
	/// @return 
	ReflectionTrait* ReflectionProperty::FindTrait(MetaType metaType) const
	{
		for (uint32_t index = 0; index < _traits.size(); ++index)
		{
			if (_traits[index]->GetMetaType() == metaType)
			{
				return _traits[index];
			}
		}
		return nullptr;
	}
}
