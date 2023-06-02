#include "HodEngine/Editor/PropertyDrawer.h"
#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"

namespace hod::editor
{
	void PropertyDrawer::DrawProperty(core::ReflectionProperty* property)
	{
		if (property->GetMetaType() == core::Reflection::Property::Variable::GetMetaTypeStatic())
		{
			core::Reflection::Property::Variable* variableProperty = static_cast<core::Reflection::Property::Variable*>(property);

			DrawPropertyVariable(variableProperty);
		}
		else if (property->GetMetaType() == core::Reflection::Property::Array::GetMetaTypeStatic())
		{
			core::Reflection::Property::Array* arrayProperty = static_cast<core::Reflection::Property::Array*>(property);

			DrawPropertyArray(arrayProperty);
		}
		//else if (property->GetType() == core::Reflection::Property::Object::_type)
		{
			//core::Reflection::Property::Object* objectProperty = static_cast<core::Reflection::Property::Object*>(property); // TODO
		}
	}

	/// @brief 
	/// @param property 
	void PropertyDrawer::DrawPropertyVariable(core::Reflection::Property::Variable* property)
	{
		core::Reflection::Property::Variable::Type type = property->GetType();

		//property
	}

	/// @brief 
	/// @param property 
	void PropertyDrawer::DrawPropertyArray(core::Reflection::Property::Array* property)
	{

	}
}
