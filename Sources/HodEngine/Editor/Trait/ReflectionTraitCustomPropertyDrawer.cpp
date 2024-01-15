#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod::editor
{
	///@brief 
	ReflectionTraitCustomPropertyDrawer::ReflectionTraitCustomPropertyDrawer(CustomPropertyDrawer* propertyDrawer)
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
	CustomPropertyDrawer* ReflectionTraitCustomPropertyDrawer::GetPropertyDrawer() const
	{
		return _propertyDrawer;
	}
}
