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
		if (property->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
		{
			ReflectionPropertyVariable* variableProperty = static_cast<ReflectionPropertyVariable*>(property);
			changed = DrawPropertyVariable(object, variableProperty);
		}
		else if (property->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic())
		{
			ReflectionPropertyArray* arrayProperty = static_cast<ReflectionPropertyArray*>(property);
			changed = DrawPropertyArray(object, arrayProperty);
		}
		else if (property->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
		{
			ReflectionPropertyObject* objectProperty = static_cast<ReflectionPropertyObject*>(property);
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
	bool PropertyDrawer::DrawPropertyVariable(void* object, ReflectionPropertyVariable* property)
	{
		ImGui::TextUnformatted(property->GetName());
		ImGui::SameLine();

		ReflectionPropertyVariable::Type type = property->GetType();

		switch (type)
		{
		case ReflectionPropertyVariable::Type::Bool:
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

		case ReflectionPropertyVariable::Type::Int8:
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

		case ReflectionPropertyVariable::Type::Float32:
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
	bool PropertyDrawer::DrawPropertyArray(void* object, ReflectionPropertyArray* property)
	{
		ImGui::TextUnformatted(property->GetName());
		ImGui::SameLine();

		ReflectionPropertyVariable::Type type = property->GetType();
		ReflectionTraitFixedSizeArray* fixedSizeArrayTrait = property->FindTrait<ReflectionTraitFixedSizeArray>();

		if (fixedSizeArrayTrait != nullptr) // Fixed size
		{
			switch (type)
			{
				case ReflectionPropertyVariable::Type::Float32:
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
	bool PropertyDrawer::DrawPropertyObject(void* object, ReflectionPropertyObject* property)
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
