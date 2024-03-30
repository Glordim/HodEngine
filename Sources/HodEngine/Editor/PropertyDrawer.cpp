#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Object.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesign.h"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod::editor
{
	bool PropertyDrawer::DrawDescriptor(void* object, ReflectionDescriptor* descriptor)
	{
		bool changed = false;

		for (ReflectionProperty* property : descriptor->GetProperties())
		{
			changed |= PropertyDrawer::DrawProperty(object, property);
		}

		return changed;
	}

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
		bool changed = false;

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(property->GetDisplayName().c_str());

		ImGui::SameLine(valuePos);

		ReflectionPropertyVariable::Type type = property->GetType();

		switch (type)
		{
		case ReflectionPropertyVariable::Type::Bool:
		{
			ImGui::PushID(property);
			bool value = property->GetValue<bool>(object);
			ImGui::SetNextItemWidth(-1);
			changed = ImGui::Checkbox("", &value);
			if (changed == true)
			{
				property->SetValue<bool>(object, value);
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Int8:
		{
			ImGui::PushID(property);
			int32_t value = property->GetValue<int8_t>(object);
			ImGui::SetNextItemWidth(-1);
			changed = ImGui::DragScalar("", ImGuiDataType_S32, &value, 1.0f);
			if (changed == true)
			{
				property->SetValue<int8_t>(object, (int8_t)value);
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Float32:
		{
			ImGui::PushID(property);
			float value = property->GetValue<float>(object);
			ImGui::SetNextItemWidth(-1);
			changed = ImGui::DragScalar("", ImGuiDataType_Float, &value, 0.01f);
			if (changed == true)
			{
				property->SetValue<float>(object, (float)value);
			}
			ImGui::PopID();
		}
		break;
		
		default:
			break;
		}

		return changed;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyArray(void* object, ReflectionPropertyArray* property)
	{
		bool changed = false;
		
		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(property->GetDisplayName().c_str());

		ImGui::SameLine(valuePos);

		ReflectionPropertyVariable::Type type = property->GetType();

		uint32_t elementCount = property->GetElementCount(object);

		ImGui::PushID(property);
		if (elementCount > 0 && ImGui::BeginTable("Items", 2) == true)
		{
			ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

			for (uint32_t index = 0; index < elementCount; ++index)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%i", index);
				ImGui::TableNextColumn();
				ImGui::PushID(index);
				switch (type)
				{
					case ReflectionPropertyVariable::Type::Float32:
					{
						float value = property->GetValue<float>(object, index);
						bool elementChanged = ImGui::DragScalar("", ImGuiDataType_Float, &value, 1.0f);
						if (elementChanged == true)
						{
							property->SetValue<float>(object, index, (float)value);
							changed = true;
						}
					}
					break;

					case ReflectionPropertyVariable::Type::Object:
					{
						if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()))
						{
							ImGui::Indent();
							void* value = property->GetValue<void*>(object, index);
							changed |= PropertyDrawer::DrawDescriptor(value, property->GetElementReflectionDescriptor());
							ImGui::Unindent();
						}
					}
					break;

					default: assert(false); break;
				}
				ImGui::PopID();

				if (index < elementCount - 1)
				{
					ImGui::SameLine();
				}
			}
			ImGui::EndTable();
		}
		ImGui::SetCursorPosX(valuePos);
		if (ImGui::Button(ICON_MD_ADD) == true)
		{
			property->InsertElement(object, elementCount);
			changed = true;
		}
		ImGui::PopID();

		return changed;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyObject(void* object, ReflectionPropertyObject* property)
	{
		bool changed = false;

		void* instance = property->GetInstance(object);
		ReflectionDescriptor* instanceDescriptor = property->GetReflectionDescriptor();

		ReflectionTraitCustomPropertyDrawer* customPropertyDrawerTrait = instanceDescriptor->FindTrait<ReflectionTraitCustomPropertyDrawer>();
		if (customPropertyDrawerTrait != nullptr)
		{
			changed = customPropertyDrawerTrait->GetPropertyDrawer()->Draw(object, property);
		}
		else
		{
			if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()) == true)
			{
				for (ReflectionProperty* property : instanceDescriptor->GetProperties())
				{
					changed |= PropertyDrawer::DrawProperty(instance, property);
				}
			}
		}
		return changed;
	}
}
