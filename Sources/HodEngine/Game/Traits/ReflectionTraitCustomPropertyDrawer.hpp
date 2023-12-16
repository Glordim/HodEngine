#pragma once

#if defined(HOD_EDITOR)

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod::game
{
	class PropertyDrawer;

	///@brief 
	class ReflectionTraitCustomPropertyDrawer : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitCustomPropertyDrawer, ReflectionTrait)

	public:

												ReflectionTraitCustomPropertyDrawer(PropertyDrawer* propertyDrawer);
												ReflectionTraitCustomPropertyDrawer(const ReflectionTraitCustomPropertyDrawer& copy) = default;
												ReflectionTraitCustomPropertyDrawer(ReflectionTraitCustomPropertyDrawer&& move) = default;
												~ReflectionTraitCustomPropertyDrawer() override;

		ReflectionTraitCustomPropertyDrawer&	operator = (const ReflectionTraitCustomPropertyDrawer& copy) = default;
		ReflectionTraitCustomPropertyDrawer&	operator = (ReflectionTraitCustomPropertyDrawer&& move) = default;

	public:

		PropertyDrawer*							GetPropertyDrawer() const;

	private:

		PropertyDrawer*							_propertyDrawer = nullptr;
	};
}

#endif
