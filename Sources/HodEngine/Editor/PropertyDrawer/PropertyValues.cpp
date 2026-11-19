#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer/PropertyValues.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param value
	PropertyValuesBase::PropertyValuesBase(void* value)
	: _values({value})
	{
	}

	/// @brief
	/// @param property
	PropertyValuesBase::PropertyValuesBase(EditorReflectedProperty& property)
	: _mixed(property.HasDifferentValues())
	, _property(&property)
	{
		_values.Reserve(property.GetInstances().Size());
		for (uint32_t index = 0; index < property.GetInstances().Size(); ++index)
		{
			_values.PushBack(property.GetObjectOf<void>(index));
		}
	}

	/// @brief
	/// @return
	uint32_t PropertyValuesBase::GetCount() const
	{
		return _values.Size();
	}

	/// @brief
	/// @param index
	/// @return
	void* PropertyValuesBase::GetValue(uint32_t index) const
	{
		return _values[index];
	}

	/// @brief
	/// @return
	bool PropertyValuesBase::IsMixed() const
	{
		return _mixed;
	}

	/// @brief
	/// @param edit
	/// @param userData
	void PropertyValuesBase::Edit(EditFunction edit, void* userData) const
	{
		// A member with a set method: given an edited copy (a set method skipping a value equal to the current one would ignore an edit in
		// place), copied with the property's own type (the drawer's type may be a base of it: WeakResourceBase for a WeakResource<Texture>)
		ReflectionPropertyObject* objectProperty = nullptr;
		if (_property != nullptr && _property->GetReflectionProperty()->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
		{
			objectProperty = static_cast<ReflectionPropertyObject*>(_property->GetReflectionProperty());
			if (objectProperty->HasSetMethod() == false || objectProperty->GetReflectionDescriptor()->CanCreateInstance() == false)
			{
				objectProperty = nullptr;
			}
		}

		for (uint32_t index = 0; index < _values.Size(); ++index)
		{
			if (objectProperty == nullptr)
			{
				edit(_values[index], userData);
				continue;
			}

			ReflectionDescriptor* descriptor = objectProperty->GetReflectionDescriptor();
			void*                 copy = descriptor->CreateInstance();
			descriptor->Copy(_values[index], copy);
			edit(copy, userData);
			objectProperty->SetValue(_property->GetInstances()[index], copy);
			descriptor->DeleteInstance(copy);
		}
	}
}
