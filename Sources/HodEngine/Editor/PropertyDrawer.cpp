#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"

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
		ReflectionProperty* reflectionProperty = reflectedProperty.GetReflectionProperty();
		if (reflectionProperty->FindTrait<ReflectionTraitHide>() != nullptr)
		{
			return false;
		}

		ImGui::PushID(reflectionProperty);
		bool changed = false;

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
	/// @tparam __TYPE__ 
	/// @param value 
	/// @return 
	template<typename __TYPE__>
	bool DrawEnum(__TYPE__& value, EnumDescriptor* enumDescriptor)
	{
		bool changed = false;

		const char* label = "NotFound";
		for (const std::pair<uint64_t, std::string>& enumValue : enumDescriptor->GetValues())
		{
			if (static_cast<__TYPE__>(enumValue.first) == value)
			{
				label = enumValue.second.c_str();
				break;
			}
		}

		if (ImGui::BeginCombo("", label))
		{
			for (const std::pair<uint64_t, std::string>& enumValue : enumDescriptor->GetValues())
			{
				if (ImGui::MenuItem(enumValue.second.c_str()) && static_cast<__TYPE__>(enumValue.first) != value)
				{
					value = static_cast<__TYPE__>(enumValue.first);
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

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
			if (ImGui::Checkbox("", &value))
			{
				property->SetValue<bool>(object, value);
				changed = true;
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Int8:
		{
			ImGui::PushID(property);
			int8_t value = property->GetValue<int8_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<int8_t>(object, (int8_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_S8, &value, 1.0f))
				{
					property->SetValue<int8_t>(object, (int8_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::UInt8:
		{
			ImGui::PushID(property);
			uint8_t value = property->GetValue<uint8_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<uint8_t>(object, (uint8_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_U8, &value, 1.0f))
				{
					property->SetValue<uint8_t>(object, (uint8_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Int16:
		{
			ImGui::PushID(property);
			int16_t value = property->GetValue<int16_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<int16_t>(object, (int16_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_S16, &value, 1.0f))
				{
					property->SetValue<int16_t>(object, (int16_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::UInt16:
		{
			ImGui::PushID(property);
			uint16_t value = property->GetValue<uint16_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<uint16_t>(object, (uint16_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_U16, &value, 1.0f))
				{
					property->SetValue<uint16_t>(object, (uint16_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Int32:
		{
			ImGui::PushID(property);
			int32_t value = property->GetValue<int32_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<int32_t>(object, (int32_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_S32, &value, 1.0f))
				{
					property->SetValue<int32_t>(object, (int32_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::UInt32:
		{
			ImGui::PushID(property);
			uint32_t value = property->GetValue<uint32_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<uint32_t>(object, (uint32_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_U32, &value, 1.0f))
				{
					property->SetValue<uint32_t>(object, (uint32_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Int64:
		{
			ImGui::PushID(property);
			int64_t value = property->GetValue<int64_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<int64_t>(object, (int64_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_S64, &value, 1.0f))
				{
					property->SetValue<int64_t>(object, (int64_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::UInt64:
		{
			ImGui::PushID(property);
			uint64_t value = property->GetValue<uint64_t>(object);
			ImGui::SetNextItemWidth(-1);
			if (property->GetEnumDescriptor() != nullptr)
			{
				if (DrawEnum(value, property->GetEnumDescriptor()))
				{
					property->SetValue<uint64_t>(object, (uint64_t)value);
					changed = true;
				}
			}
			else
			{
				if (ImGui::DragScalar("", ImGuiDataType_U64, &value, 1.0f))
				{
					property->SetValue<uint64_t>(object, (uint64_t)value);
					changed = true;
				}
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Float32:
		{
			ImGui::PushID(property);
			float value = property->GetValue<float>(object);
			ImGui::SetNextItemWidth(-1);
			if (ImGui::DragScalar("", ImGuiDataType_Float, &value, 0.01f))
			{
				property->SetValue<float>(object, (float)value);
				changed = true;
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::Float64:
		{
			ImGui::PushID(property);
			double value = property->GetValue<double>(object);
			ImGui::SetNextItemWidth(-1);
			if (ImGui::DragScalar("", ImGuiDataType_Double, &value, 0.01f))
			{
				property->SetValue<double>(object, (double)value);
				changed = true;
			}
			ImGui::PopID();
		}
		break;

		case ReflectionPropertyVariable::Type::String:
		{
			ImGui::PushID(property);
			std::string value = property->GetValue<std::string>(object);
			value.reserve(512);
			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputText("", value.data(), value.capacity()))
			{
				value = value.c_str();
				property->SetValue<std::string>(object, value);
				changed = true;
			}
			ImGui::PopID();
		}
		break;
		
		default:
		{
			ImGui::TextUnformatted("Unsupported type");
		}
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

					case ReflectionPropertyVariable::Type::Object:
					{
						if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()))
						{
							ImGui::Indent();
							void* value = property->GetValue<void*>(object, index);
							//changed |= PropertyDrawer::DrawDescriptor(value, property->GetElementReflectionDescriptor());
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
