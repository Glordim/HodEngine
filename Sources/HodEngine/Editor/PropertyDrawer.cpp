#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

namespace hod::editor
{
	bool PropertyDrawer::DrawDescriptor(EditorReflectedObject& reflectedObject)
	{
		bool changed = false;

		for (EditorReflectedProperty* property : reflectedObject.GetProperties())
		{
			changed |= PropertyDrawer::DrawProperty(*property);
		}

		return changed;
	}

	bool PropertyDrawer::DrawProperty(EditorReflectedProperty& reflectedProperty)
	{
		ImGui::PushID(reflectedProperty.GetReflectionProperty());
		bool changed = false;

		ReflectionProperty* reflectionProperty = reflectedProperty.GetReflectionProperty();

		if (reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
		{
			changed = DrawPropertyVariable(reflectedProperty);
		}
		else if (reflectionProperty->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic())
		{
			changed = DrawPropertyArray(reflectedProperty);
		}
		else if (reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
		{
			changed = DrawPropertyObject(reflectedProperty);

			if (changed == true)
			{
				//objectProperty->SetValue(object, objectProperty->GetValue(object));
			}
		}
		ImGui::PopID();

		return changed;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyVariable(EditorReflectedProperty& reflectedProperty)
	{
		bool changed = false;
		changed |= BeginProperty(reflectedProperty);

		ReflectionPropertyVariable* property = static_cast<ReflectionPropertyVariable*>(reflectedProperty.GetReflectionProperty());
		void* object = reflectedProperty.GetInstance();

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

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
	bool PropertyDrawer::DrawPropertyArray(EditorReflectedProperty& reflectedProperty)
	{
		ReflectionPropertyArray* property = static_cast<ReflectionPropertyArray*>(reflectedProperty.GetReflectionProperty());
		void* object = reflectedProperty.GetInstance();

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

					/*
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
					*/

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
		if (ImGui::Button(ICON_MDI_PLUS) == true)
		{
			property->InsertElement(object, elementCount);
			changed = true;
		}
		ImGui::PopID();

		return changed;
	}

	/// @brief 
	/// @param property 
	bool PropertyDrawer::DrawPropertyObject(EditorReflectedProperty& reflectedProperty)
	{
		EditorReflectedObject* subEditorReflectedObject = reflectedProperty.GetEditorReflectedObject();

		ReflectionPropertyObject* property = static_cast<ReflectionPropertyObject*>(reflectedProperty.GetReflectionProperty());

		bool changed = false;
		ReflectionDescriptor* instanceDescriptor = property->GetReflectionDescriptor();

		ReflectionTraitCustomPropertyDrawer* customPropertyDrawerTrait = instanceDescriptor->FindTrait<ReflectionTraitCustomPropertyDrawer>();
		if (customPropertyDrawerTrait != nullptr)
		{
			changed = customPropertyDrawerTrait->GetPropertyDrawer()->Draw(*subEditorReflectedObject);
		}
		else
		{
			if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()) == true)
			{
				for (EditorReflectedProperty* subEditorReflectedProperty : subEditorReflectedObject->GetProperties())
				{
					changed |= PropertyDrawer::DrawProperty(*subEditorReflectedProperty);
				}
			}
		}
		return changed;
	}

	bool PropertyDrawer::BeginProperty(EditorReflectedProperty& reflectedProperty)
	{
		bool changed = false;
		bool isOverride = reflectedProperty.IsOverride();
		if (isOverride)
		{
			float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(reflectedProperty.GetReflectionProperty()->GetDisplayName().c_str());
		if (isOverride && ImGui::BeginPopupContextItem("OverrideContext") == true)
		{
			if (ImGui::Button("Revert") == true)
			{
				ReflectionProperty* reflectionProperty = reflectedProperty.GetReflectionProperty();
				reflectionProperty->Copy(reflectedProperty.GetSourceInstance(), reflectedProperty.GetInstance());
				changed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return changed;
	}
}
