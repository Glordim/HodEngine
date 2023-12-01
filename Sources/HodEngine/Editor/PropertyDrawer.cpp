#include "HodEngine/Editor/PropertyDrawer.h"
#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"
#include "HodEngine/Core/Object.h"

#include "HodEngine/Core/Reflection/ReflectionTrait.h"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitFixedSizeArray.h"

#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::editor
{
	bool PropertyDrawer::DrawProperty(void* object, ReflectionProperty* property)
	{
		ImGui::PushID(object);
		bool changed = false;
		if (property->GetMetaType() == Reflection::Property::Variable::GetMetaTypeStatic())
		{
			Reflection::Property::Variable* variableProperty = static_cast<Reflection::Property::Variable*>(property);
			changed = DrawPropertyVariable(object, variableProperty);
		}
		else if (property->GetMetaType() == Reflection::Property::Array::GetMetaTypeStatic())
		{
			Reflection::Property::Array* arrayProperty = static_cast<Reflection::Property::Array*>(property);
			changed = DrawPropertyArray(object, arrayProperty);
		}
		else if (property->GetMetaType() == Reflection::Property::Object::GetMetaTypeStatic())
		{
			Reflection::Property::Object* objectProperty = static_cast<Reflection::Property::Object*>(property);
			changed = DrawPropertyObject(object, objectProperty);

			if (changed == true)
			{
				objectProperty->SetValue(object, objectProperty->GetValue(object));
			}
		}
		ImGui::PopID();

		return changed;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyVariable(void* object, Reflection::Property::Variable* property)
	{
		ImGui::TextUnformatted(property->GetName());
		ImGui::SameLine();

		Reflection::Property::Variable::Type type = property->GetType();

		switch (type)
		{
		case Reflection::Property::Variable::Type::Bool:
		{
			ImGui::PushID(property);
			bool value = property->GetValue<bool>(object);
			bool changed = ImGui::Checkbox("", &value);
			if (changed == true)
			{
				property->SetValue<bool>(object, value);
			}
			ImGui::PopID();
			return changed;
		}
		break;

		case Reflection::Property::Variable::Type::Int8:
		{
			ImGui::PushID(property);
			int32_t value = property->GetValue<int8_t>(object);
			bool changed = ImGui::DragScalar("", ImGuiDataType_S32, &value, 1.0f);
			if (changed == true)
			{
				property->SetValue<int8_t>(object, (int8_t)value);
			}
			ImGui::PopID();
			return changed;
		}
		break;

		case Reflection::Property::Variable::Type::Float32:
		{
			ImGui::PushID(property);
			float value = property->GetValue<float>(object);
			bool changed = ImGui::DragScalar("", ImGuiDataType_Float, &value, 0.01f);
			if (changed == true)
			{
				property->SetValue<float>(object, (float)value);
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
	bool PropertyDrawer::DrawPropertyArray(void* object, Reflection::Property::Array* property)
	{
		ImGui::TextUnformatted(property->GetName());
		ImGui::SameLine();

		Reflection::Property::Variable::Type type = property->GetType();
		ReflectionTraitFixedSizeArray* fixedSizeArrayTrait = property->FindTrait<ReflectionTraitFixedSizeArray>();

		if (fixedSizeArrayTrait != nullptr) // Fixed size
		{
			switch (type)
			{
				case Reflection::Property::Variable::Type::Float32:
				{
					bool changed = false;

					ImGui::PushID(property);
					for (uint32_t index = 0; index < fixedSizeArrayTrait->GetFixedSize(); ++index)
					{
						ImGui::Text("%i", index);
						ImGui::SameLine();
						ImGui::PushID(index);
						float value = property->GetValue<float>(object, index);
						bool elementChanged = ImGui::DragScalar("", ImGuiDataType_Float, &value, 1.0f);
						if (elementChanged == true)
						{
							property->SetValue<float>(object, index, (float)value);
							changed = true;
						}
						ImGui::PopID();

						if (index < fixedSizeArrayTrait->GetFixedSize() - 1)
						{
							ImGui::SameLine();
						}
					}
					
					ImGui::PopID();
					return changed;
				}
				break;

				default: break;
			}
		}
		else
		{
			// todo
		}

		return false;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyObject(void* object, Reflection::Property::Object* property)
	{
		if (ImGui::CollapsingHeader(property->GetName()) == true)
		{
			void* instance = property->GetInstance(object);
			ReflectionDescriptor* instanceDescriptor = property->GetReflectionDescriptor();

			bool changed = false;

			for (ReflectionProperty* property : instanceDescriptor->GetProperties())
			{
				changed |= PropertyDrawer::DrawProperty(instance, property);
			}

			return changed;
		}

		return false;
	}
}
