#include "HodEngine/Editor/Pch.hpp"
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
		DefaultAllocator::GetInstance().Delete(_propertyDrawer);
	}

	/// @brief 
	/// @return 
	CustomPropertyDrawer* ReflectionTraitCustomPropertyDrawer::GetPropertyDrawer() const
	{
		return _propertyDrawer;
	}
}
