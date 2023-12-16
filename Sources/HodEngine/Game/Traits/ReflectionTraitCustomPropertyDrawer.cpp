#if defined(HOD_EDITOR)

#include "HodEngine/Game/Traits/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Game/Traits/PropertyDrawer.hpp"

namespace hod::game
{
	///@brief 
	ReflectionTraitCustomPropertyDrawer::ReflectionTraitCustomPropertyDrawer(PropertyDrawer* propertyDrawer)
	: ReflectionTrait()
	, _propertyDrawer(propertyDrawer)
	{
	}

	/// @brief 
	ReflectionTraitCustomPropertyDrawer::~ReflectionTraitCustomPropertyDrawer()
	{
		delete _propertyDrawer;
	}

	/// @brief 
	/// @return 
	PropertyDrawer* ReflectionTraitCustomPropertyDrawer::GetPropertyDrawer() const
	{
		return _propertyDrawer;
	}
}

#endif
