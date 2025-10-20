#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"

#include "HodEngine/Editor/Trait/ReflectionTraitCustomPropertyDrawer.hpp"

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
				// objectProperty->SetValue(object, objectProperty->GetValue(object));
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
		for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
		{
			if (static_cast<__TYPE__>(enumValue.first) == value)
			{
				label = enumValue.second.CStr();
				break;
			}
		}

		if (ImGui::BeginCombo("", label))
		{
			for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
			{
				if (ImGui::MenuItem(enumValue.second.CStr()) && static_cast<__TYPE__>(enumValue.first) != value)
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
	bool PropertyDrawer::DrawPropertyVariable(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		ReflectionPropertyVariable* property = static_cast<ReflectionPropertyVariable*>(editorReflectedProperty.GetReflectionProperty());
		switch (property->GetType())
		{
			case ReflectionPropertyVariable::Type::Bool:
			{
				ImGui::PushID(property);
				bool value = editorReflectedProperty.GetValue<bool>();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Checkbox("", &value))
				{
					editorReflectedProperty.SetValue(value);
					changed = true;
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Int8:
			{
				ImGui::PushID(property);
				int8_t value = editorReflectedProperty.GetValue<int8_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<int8_t>((int8_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_S8, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<int8_t>((int8_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::UInt8:
			{
				ImGui::PushID(property);
				uint8_t value = editorReflectedProperty.GetValue<uint8_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<uint8_t>((uint8_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_U8, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<uint8_t>((uint8_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Int16:
			{
				ImGui::PushID(property);
				int16_t value = editorReflectedProperty.GetValue<int16_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<int16_t>((int16_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_S16, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<int16_t>((int16_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::UInt16:
			{
				ImGui::PushID(property);
				uint16_t value = editorReflectedProperty.GetValue<uint16_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<uint16_t>((uint16_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_U16, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<uint16_t>((uint16_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Int32:
			{
				ImGui::PushID(property);
				int32_t value = editorReflectedProperty.GetValue<int32_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<int32_t>((int32_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_S32, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<int32_t>((int32_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::UInt32:
			{
				ImGui::PushID(property);
				uint32_t value = editorReflectedProperty.GetValue<uint32_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<uint32_t>((uint32_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_U32, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<uint32_t>((uint32_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Int64:
			{
				ImGui::PushID(property);
				int64_t value = editorReflectedProperty.GetValue<int64_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<int64_t>((int64_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_S64, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<int64_t>((int64_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::UInt64:
			{
				ImGui::PushID(property);
				uint64_t value = editorReflectedProperty.GetValue<uint64_t>();
				ImGui::SetNextItemWidth(-1);
				if (property->GetEnumDescriptor() != nullptr)
				{
					if (DrawEnum(value, property->GetEnumDescriptor()))
					{
						editorReflectedProperty.SetValue<uint64_t>((uint64_t)value);
						changed = true;
					}
				}
				else
				{
					if (ImGui::DragScalar("", ImGuiDataType_U64, &value, 1.0f))
					{
						editorReflectedProperty.SetValue<uint64_t>((uint64_t)value);
						changed = true;
					}
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Float32:
			{
				ImGui::PushID(property);
				float value = editorReflectedProperty.GetValue<float>();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragScalar("", ImGuiDataType_Float, &value, 0.01f))
				{
					editorReflectedProperty.SetValue<float>((float)value);
					changed = true;
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::Float64:
			{
				ImGui::PushID(property);
				double value = editorReflectedProperty.GetValue<double>();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::DragScalar("", ImGuiDataType_Double, &value, 0.01f))
				{
					editorReflectedProperty.SetValue<double>((double)value);
					changed = true;
				}
				ImGui::PopID();
			}
			break;

			case ReflectionPropertyVariable::Type::String:
			{
				ImGui::PushID(property);
				String value = editorReflectedProperty.GetValue<String>();
				value.Reserve(512);
				ImGui::SetNextItemWidth(-1);
				if (ImGui::InputText("", value.Data(), value.Capacity()))
				{
					value = value.CStr();
					editorReflectedProperty.SetValue<String>(value);
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
	bool PropertyDrawer::DrawPropertyArray(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		float cursorX = ImGui::GetCursorPosX();

		ReflectionPropertyArray*         property = static_cast<ReflectionPropertyArray*>(editorReflectedProperty.GetReflectionProperty());
		ReflectionPropertyVariable::Type type = property->GetType();

		void* object = editorReflectedProperty.GetInstance();

		uint32_t elementCount = editorReflectedProperty.GetArraySize();

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
						float value = editorReflectedProperty.GetValueAtIndex<float>(index);
						// float value = property->GetValue<float>(object, index);
						bool elementChanged = ImGui::DragScalar("", ImGuiDataType_Float, &value, 1.0f);
						if (elementChanged == true)
						{
							editorReflectedProperty.SetValueAtIndex<float>(index, (float)value);
							changed = true;
						}
					}
					break;

					case ReflectionPropertyVariable::Type::Object:
					{
						EditorReflectedProperty elementProperty = editorReflectedProperty.GenerateElementProperty(index);
						changed |= PropertyDrawer::DrawPropertyObject(elementProperty);

						/*
						void* value = property->GetValue<void*>(object, index);
						EditorReflectedObject subEditorReflectedObject(value, property->GetElementReflectionDescriptor(), nullptr);
						if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()))
						{
						    ImGui::Indent();
						    //void* value = property->GetValue<void*>(object, index);
						    changed |= PropertyDrawer::DrawDescriptor(subEditorReflectedObject);
						    ImGui::Unindent();
						}
						*/
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
		ImGui::SetCursorPosX(cursorX);
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
		ReflectionDescriptor* instanceDescriptor;

		ReflectionProperty* property = reflectedProperty.GetReflectionProperty();
		if (property->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
		{
			ReflectionPropertyObject* propertyObject = static_cast<ReflectionPropertyObject*>(property);
			instanceDescriptor = propertyObject->GetReflectionDescriptor();
		}
		else if (property->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic())
		{
			ReflectionPropertyArray* propertyArray = static_cast<ReflectionPropertyArray*>(property);
			instanceDescriptor = propertyArray->GetElementReflectionDescriptor();
		}
		else
		{
			assert(false);
			return false;
		}

		bool changed = false;

		ReflectionTraitCustomPropertyDrawer* customPropertyDrawerTrait = instanceDescriptor->FindTrait<ReflectionTraitCustomPropertyDrawer>();
		if (customPropertyDrawerTrait != nullptr)
		{
			changed = customPropertyDrawerTrait->GetPropertyDrawer()->Draw(reflectedProperty);
		}
		else
		{
			if (ImGui::CollapsingHeader(property->GetDisplayName().CStr()) == true)
			{
				EditorReflectedObject* subEditorReflectedObject = reflectedProperty.GetEditorReflectedObject();
				for (EditorReflectedProperty* subEditorReflectedProperty : subEditorReflectedObject->GetProperties())
				{
					changed |= PropertyDrawer::DrawProperty(*subEditorReflectedProperty);
				}
			}
		}
		return changed;
	}

	bool PropertyDrawer::BeginProperty(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		editorReflectedProperty.GetEditorReflectedObject(); // todo remove...
		bool isOverride = editorReflectedProperty.IsOverride();
		if (isOverride)
		{
			float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y),
			                                          ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(editorReflectedProperty.GetReflectionProperty()->GetDisplayName().CStr());
		if (isOverride && ImGui::BeginPopupContextItem("OverrideContext") == true)
		{
			if (ImGui::Button("Revert") == true)
			{
				ReflectionProperty* reflectionProperty = editorReflectedProperty.GetReflectionProperty();
				reflectionProperty->Copy(editorReflectedProperty.GetSourceInstance(), editorReflectedProperty.GetInstance());
				changed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return changed;
	}
}
