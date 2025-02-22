#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod::editor
{
	class CustomPropertyDrawer;

	///@brief 
	class HOD_EDITOR_API ReflectionTraitCustomPropertyDrawer : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitCustomPropertyDrawer, ReflectionTrait)

	public:

												ReflectionTraitCustomPropertyDrawer(CustomPropertyDrawer* propertyDrawer);
												ReflectionTraitCustomPropertyDrawer(const ReflectionTraitCustomPropertyDrawer& copy) = default;
												ReflectionTraitCustomPropertyDrawer(ReflectionTraitCustomPropertyDrawer&& move) = default;
												~ReflectionTraitCustomPropertyDrawer() override;

		ReflectionTraitCustomPropertyDrawer&	operator = (const ReflectionTraitCustomPropertyDrawer& copy) = default;
		ReflectionTraitCustomPropertyDrawer&	operator = (ReflectionTraitCustomPropertyDrawer&& move) = default;

	public:

		CustomPropertyDrawer*					GetPropertyDrawer() const;

	private:

		CustomPropertyDrawer*					_propertyDrawer = nullptr;
	};
}
