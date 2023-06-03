#include "HodEngine/Editor/PropertyDrawer.h"
#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"

#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::editor
{
	bool PropertyDrawer::DrawProperty(Object& object, core::ReflectionProperty* property)
	{
		if (property->GetMetaType() == core::Reflection::Property::Variable::GetMetaTypeStatic())
		{
			core::Reflection::Property::Variable* variableProperty = static_cast<core::Reflection::Property::Variable*>(property);

			return DrawPropertyVariable(object, variableProperty);
		}
		else if (property->GetMetaType() == core::Reflection::Property::Array::GetMetaTypeStatic())
		{
			core::Reflection::Property::Array* arrayProperty = static_cast<core::Reflection::Property::Array*>(property);

			return DrawPropertyArray(object, arrayProperty);
		}
		//else if (property->GetType() == core::Reflection::Property::Object::_type)
		{
			//core::Reflection::Property::Object* objectProperty = static_cast<core::Reflection::Property::Object*>(property); // TODO
		}

		return false;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyVariable(Object& object, core::Reflection::Property::Variable* property)
	{
		core::Reflection::Property::Variable::Type type = property->GetType();

		switch (type)
		{
		case core::Reflection::Property::Variable::Type::Bool:
		{
			bool value = property->GetValue<bool>(&object);
			ImGui::PushID(property);
			bool changed = ImGui::Checkbox(property->GetName(), &value);
			if (changed == true)
			{
				property->SetValue<bool>(&object, value);
			}
			ImGui::PopID();
			return changed;
		}
		break;

		case core::Reflection::Property::Variable::Type::Int8:
		{
			int32_t value = property->GetValue<int8_t>(&object);
			ImGui::PushID(property);
			bool changed = ImGui::DragScalar(property->GetName(), ImGuiDataType_S32, &value, 1.0f);
			if (changed == true)
			{
				property->SetValue<int8_t>(&object, (int8_t)value);
			}
			ImGui::PopID();
			return changed;
		}
		break;
		
		default:
			break;
		}

		return false;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyArray(Object& object, core::Reflection::Property::Array* property)
	{
		return false;
	}
}
