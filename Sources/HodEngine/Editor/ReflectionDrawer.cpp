#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitVisibleIf.hpp"
#include "HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp"
#include "HodEngine/Editor/PropertyDrawer/PropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/PropertyHighlight.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"
#include "HodEngine/ImGui/Widgets.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include <cmath>
#include <unordered_map>

namespace hod::inline editor
{
	namespace
	{
		std::unordered_map<const EnumDescriptor*, ReflectionDrawer::EnumValuePreview> enumValuePreviews;

		/// @brief The frame of a highlighted property around min/max, pulsing (its alpha and its margin) in the color of the asset browser's ping
		void DrawHighlightFrame(const ImVec2& min, const ImVec2& max)
		{
			constexpr float pulseSpeed = 6.0f; // About once a second
			float           pulse = 0.5f + 0.5f * std::cos((float)ImGui::GetTime() * pulseSpeed);
			float           margin = 1.0f + 2.0f * pulse;
			ImU32           color = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, 0.4f + 0.6f * pulse));
			ImGui::GetWindowDrawList()->AddRect(ImVec2(min.x - margin, min.y - margin), ImVec2(max.x + margin, max.y + margin), color, ImGui::GetStyle().FrameRounding, 2.0f);
		}

		/// @brief The disabled property's row hovered (not an ImGui item: its widgets are), for its tooltip
		struct DisabledRowHover
		{
			const void* _property = nullptr;
			const void* _owner = nullptr;
			double      _start = 0.0;
			bool        _stationary = false;
			int         _lastFrame = -1;
		};

		/// @return Whether the tooltip of the row can show, as an item's (SetItemTooltip: the mouse stationary once, then style.HoverDelayShort)
		bool IsDisabledRowTooltipReady(const void* property, const void* owner)
		{
			static DisabledRowHover hover;

			int frame = ImGui::GetFrameCount();
			if (hover._property != property || hover._owner != owner || hover._lastFrame < frame - 1)
			{
				hover = DisabledRowHover {property, owner, ImGui::GetTime(), false, frame};
			}
			hover._lastFrame = frame;

			const ImGuiContext& context = *ImGui::GetCurrentContext();
			hover._stationary |= context.MouseStationaryTimer >= context.Style.HoverStationaryDelay;
			return hover._stationary && ImGui::GetTime() - hover._start >= context.Style.HoverDelayShort;
		}

		/// @return The editor's highlighted properties, up to date with the ImGui frame
		PropertyHighlight& GetPropertyHighlight()
		{
			static PropertyHighlight propertyHighlight;
			static int               frame = -1;
			if (frame != ImGui::GetFrameCount())
			{
				frame = ImGui::GetFrameCount();
				propertyHighlight.NewFrame();
			}
			return propertyHighlight;
		}

		void DrawEnumValuePreview(ReflectionDrawer::EnumValuePreview preview, uint64_t value, const char* label)
		{
			if (preview != nullptr && ImGui::BeginItemTooltip())
			{
				if (label != nullptr)
				{
					ImGui::TextUnformatted(label);
				}
				preview(value);
				ImGui::EndTooltip();
			}
		}
	}

	/// @brief
	/// @param enumDescriptor
	/// @param preview
	/// @return
	bool ReflectionDrawer::RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, EnumValuePreview preview)
	{
		if (enumValuePreviews.emplace(&enumDescriptor, preview).second == false)
		{
			OUTPUT_ERROR("ReflectionDrawer::RegisterEnumValuePreview: the enum has a preview already");
			return false;
		}
		return true;
	}

	/// @brief
	/// @param enumDescriptor
	void ReflectionDrawer::UnregisterEnumValuePreview(const EnumDescriptor& enumDescriptor)
	{
		enumValuePreviews.erase(&enumDescriptor);
	}

	bool ReflectionDrawer::DrawDescriptor(EditorReflectedObject& reflectedObject)
	{
		bool changed = false;

		for (EditorReflectedProperty* property : reflectedObject.GetProperties())
		{
			changed |= ReflectionDrawer::DrawProperty(*property);
		}

		return changed;
	}

	bool ReflectionDrawer::DrawProperty(EditorReflectedProperty& reflectedProperty, bool onlyValue)
	{
		ReflectionProperty* reflectionProperty = reflectedProperty.GetReflectionProperty();
		if (reflectionProperty->FindTrait<ReflectionTraitHide>() != nullptr || IsPropertyVisible(reflectedProperty) == false)
		{
			return false;
		}

		ImGui::PushID(reflectionProperty);
		bool changed = false;
		bool enabled = IsPropertyEnabled(reflectedProperty);
		// Its row (no group: SameLine's offsets would start from it), measured for the hover of a disabled property and the highlight frame
		ImVec2 rowMin = ImGui::GetCursorScreenPos();
		float  rowWidth = ImGui::GetContentRegionAvail().x;
		ImGui::BeginDisabled(enabled == false);

		if (reflectionProperty->GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
		{
			changed = DrawPropertyVariable(reflectedProperty, onlyValue);
		}
		else if (reflectionProperty->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic())
		{
			changed = DrawPropertyArray(reflectedProperty, onlyValue);
		}
		else if (reflectionProperty->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
		{
			changed = DrawPropertyObject(reflectedProperty, onlyValue);

			if (changed == true)
			{
				// objectProperty->SetValue(object, objectProperty->GetValue(object));
			}
		}
		ImGui::EndDisabled();
		ImVec2 rowMax(rowMin.x + rowWidth, Max(ImGui::GetItemRectMax().y, rowMin.y + ImGui::GetFrameHeight()));
		// Hovered long enough for a tooltip: why it is disabled, and the property deciding it highlighted at the same time
		if (enabled == false && ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(rowMin, rowMax) &&
		    IsDisabledRowTooltipReady(reflectionProperty, reflectedProperty.GetInstance()))
		{
			ShowDisabledReason(reflectedProperty);
		}
		if (IsHighlighted(reflectedProperty))
		{
			DrawHighlightFrame(rowMin, rowMax);
		}
		ImGui::PopID();

		return changed;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	/// @return
	template<typename __TYPE__>
	bool DrawEnum(__TYPE__& value, EnumDescriptor* enumDescriptor, bool mixed)
	{
		bool changed = false;

		const char* label = mixed ? GetMixedValueLabel() : "NotFound";
		for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
		{
			if (mixed == false && static_cast<__TYPE__>(enumValue.first) == value)
			{
				label = enumValue.second.CStr();
				break;
			}
		}

		auto                             it = enumValuePreviews.find(enumDescriptor);
		ReflectionDrawer::EnumValuePreview preview = (it != enumValuePreviews.end()) ? it->second : nullptr;

		if (ImGui::BeginCombo("", label))
		{
			for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
			{
				// A mixed value applies even the first instance's value: to the other ones
				if (ImGui::MenuItem(enumValue.second.CStr()) && (mixed || static_cast<__TYPE__>(enumValue.first) != value))
				{
					value = static_cast<__TYPE__>(enumValue.first);
					changed = true;
				}
				DrawEnumValuePreview(preview, enumValue.first, nullptr);
			}
			ImGui::EndCombo();
		}
		else if (mixed == false)
		{
			DrawEnumValuePreview(preview, static_cast<uint64_t>(value), label);
		}

		return changed;
	}

	/// @brief
	/// @param property
	bool ReflectionDrawer::DrawPropertyVariable(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		// The instances edited together differ: the widget shows a mixed value (see ImGuiItemFlags_MixedValue), an edit applies to them all
		bool mixed = editorReflectedProperty.HasDifferentValues();
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);

		ReflectionPropertyVariable* property = static_cast<ReflectionPropertyVariable*>(editorReflectedProperty.GetReflectionProperty());
		switch (property->GetType())
		{
			case ReflectionPropertyVariable::Type::Bool:
			{
				ImGui::PushID(property);
				bool value = (mixed == false) && editorReflectedProperty.GetValue<bool>(); // a click on a mixed value checks them all
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
					if (DrawEnum(value, property->GetEnumDescriptor(), mixed))
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
		ImGui::PopItemFlag();

		return changed;
	}

	/// @brief
	/// @param property
	bool ReflectionDrawer::DrawPropertyArray(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		// Their elements and size are edited on the first instance only for now
		if (editorReflectedProperty.IsEditingMultipleInstance())
		{
			ImGui::TextDisabled("%s", GetMixedValueLabel());
			ImGui::SetItemTooltip("An array is edited one object at a time");
			return changed;
		}

		float cursorX = ImGui::GetCursorPosX();

		ReflectionPropertyArray*         property = static_cast<ReflectionPropertyArray*>(editorReflectedProperty.GetReflectionProperty());
		ReflectionPropertyVariable::Type type = property->GetType();

		void* object = editorReflectedProperty.GetInstance();

		uint32_t elementCount = editorReflectedProperty.GetArraySize();

		ImGui::PushID(property);
		if (elementCount > 0 && ImGui::BeginTable("Items", 2, ImGuiTableFlags_PadOuterX) == true)
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
						changed |= ReflectionDrawer::DrawPropertyObject(elementProperty);

						/*
						void* value = property->GetValue<void*>(object, index);
						EditorReflectedObject subEditorReflectedObject(value, property->GetElementReflectionDescriptor(), nullptr);
						if (ImGui::CollapsingHeader(property->GetDisplayName().c_str()))
						{
						    ImGui::Indent();
						    //void* value = property->GetValue<void*>(object, index);
						    changed |= ReflectionDrawer::DrawDescriptor(subEditorReflectedObject);
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
	bool ReflectionDrawer::DrawPropertyObject(EditorReflectedProperty& reflectedProperty, bool onlyValue)
	{
		ReflectionDescriptor* instanceDescriptor;

		ReflectionProperty* property = reflectedProperty.GetReflectionProperty();
		if (property->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
		{
			ReflectionPropertyObject* propertyObject = static_cast<ReflectionPropertyObject*>(property);
			instanceDescriptor = propertyObject->GetReflectionDescriptor();
		}
		else if (property->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic())
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

		PropertyDrawerBase* propertyDrawer = PropertyDrawerRegistry::Find(*instanceDescriptor);
		if (propertyDrawer != nullptr)
		{
			if (onlyValue == false)
			{
				changed |= BeginProperty(reflectedProperty);
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
			}
			PropertyValuesBase values(reflectedProperty);
			changed |= propertyDrawer->Draw(values);
		}
		else
		{
			bool opened = ImGui::CollapsingHeader(property->GetDisplayName().CStr());
			if (onlyValue == false)
			{
				changed |= DrawContextMenu(reflectedProperty);
			}
			if (opened == true)
			{
				EditorReflectedObject* subEditorReflectedObject = reflectedProperty.GetEditorReflectedObject();
				for (EditorReflectedProperty* subEditorReflectedProperty : subEditorReflectedObject->GetProperties())
				{
					changed |= ReflectionDrawer::DrawProperty(*subEditorReflectedProperty);
				}
			}
		}
		return changed;
	}

	/// @brief
	/// @param value
	/// @param descriptor
	/// @return
	bool ReflectionDrawer::DrawValue(void* value, const ReflectionDescriptor& descriptor)
	{
		ImGui::PushID(value);
		bool changed = false;

		PropertyDrawerBase* propertyDrawer = PropertyDrawerRegistry::Find(descriptor);
		if (propertyDrawer != nullptr)
		{
			PropertyValuesBase values(value);
			changed = propertyDrawer->Draw(values);
		}
		else
		{
			EditorReflectedObject reflectedObject(value, &descriptor, nullptr, nullptr);
			changed = DrawDescriptor(reflectedObject);
		}

		ImGui::PopID();
		return changed;
	}

	bool ReflectionDrawer::BeginProperty(EditorReflectedProperty& editorReflectedProperty)
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
		changed |= DrawContextMenu(editorReflectedProperty);

		return changed;
	}

	/// @brief
	/// @param owner
	/// @param property
	void ReflectionDrawer::RequestHighlight(const void* owner, std::string_view property)
	{
		GetPropertyHighlight().Request(owner, property);
	}

	/// @brief
	/// @param editorReflectedProperty
	/// @return
	bool ReflectionDrawer::IsHighlighted(const EditorReflectedProperty& editorReflectedProperty)
	{
		if (editorReflectedProperty.GetInstances().Empty())
		{
			return false;
		}
		return GetPropertyHighlight().IsHighlighted(editorReflectedProperty.GetInstances()[0], editorReflectedProperty.GetReflectionProperty()->GetName());
	}

	/// @brief
	/// @param editorReflectedProperty
	void ReflectionDrawer::ShowDisabledReason(const EditorReflectedProperty& editorReflectedProperty)
	{
		ReflectionTraitEnabledIf* enabledIf = editorReflectedProperty.GetReflectionProperty()->FindTrait<ReflectionTraitEnabledIf>();
		if (enabledIf == nullptr)
		{
			return;
		}
		if (enabledIf->GetReason().Empty() == false)
		{
			ImGui::SetTooltip("%s", enabledIf->GetReason().CStr());
		}
		for (const String& decidingProperty : enabledIf->GetDecidingProperties())
		{
			RequestHighlight(editorReflectedProperty.GetInstance(), decidingProperty);
		}
	}

	/// @brief
	void ReflectionDrawer::HighlightLastItem()
	{
		DrawHighlightFrame(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	}

	/// @brief
	/// @param min
	/// @param max
	void ReflectionDrawer::HighlightRect(const Vector2& min, const Vector2& max)
	{
		DrawHighlightFrame(ImVec2(min.GetX(), min.GetY()), ImVec2(max.GetX(), max.GetY()));
	}

	/// @brief
	/// @param editorReflectedProperty
	/// @return
	bool ReflectionDrawer::IsPropertyVisible(const EditorReflectedProperty& editorReflectedProperty)
	{
		ReflectionTraitVisibleIf* visibleIf = editorReflectedProperty.GetReflectionProperty()->FindTrait<ReflectionTraitVisibleIf>();
		if (visibleIf == nullptr)
		{
			return true;
		}
		for (const void* owner : editorReflectedProperty.GetInstances())
		{
			if (visibleIf->IsVisible(owner))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param editorReflectedProperty
	/// @return
	bool ReflectionDrawer::IsPropertyEnabled(const EditorReflectedProperty& editorReflectedProperty)
	{
		ReflectionTraitEnabledIf* enabledIf = editorReflectedProperty.GetReflectionProperty()->FindTrait<ReflectionTraitEnabledIf>();
		if (enabledIf == nullptr)
		{
			return true;
		}
		for (const void* owner : editorReflectedProperty.GetInstances())
		{
			if (enabledIf->IsEnabled(owner))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief Find the root object owning the property and the chain of object properties leading from it to the property owner
	/// @return nullptr if the owner can't be resolved from a default constructed root (array element, no default constructor...)
	static const ReflectionDescriptor* FindDefaultRoot(EditorReflectedProperty& editorReflectedProperty, Vector<ReflectionPropertyObject*>& path)
	{
		ReflectionProperty* reflectionProperty = editorReflectedProperty.GetReflectionProperty();
		if (reflectionProperty->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic())
		{
			return nullptr; // ReflectionPropertyArray::Copy is not implemented
		}

		EditorReflectedObject* root = editorReflectedProperty.GetParent();
		while (root != nullptr && root->GetSourceProperty() != nullptr)
		{
			ReflectionProperty* sourceProperty = root->GetSourceProperty()->GetReflectionProperty();
			if (sourceProperty->GetRttiType() != ReflectionPropertyObject::GetRttiTypeStatic())
			{
				return nullptr;
			}
			path.push_back(static_cast<ReflectionPropertyObject*>(sourceProperty));
			root = root->GetSourceProperty()->GetParent();
		}

		if (root == nullptr || root->GetReflectionDescriptor() == nullptr || root->GetReflectionDescriptor()->CanCreateInstance() == false)
		{
			return nullptr;
		}
		return root->GetReflectionDescriptor();
	}

	/// @brief Copies the property's value from source to destination through its set method, when it has one, so the owner
	/// reacts as to any edit (e.g. a ui node marked for a new layout pass). ReflectionPropertyObject::Copy writes the object
	/// in place, bypassing it.
	static void CopyThroughSetter(ReflectionProperty* reflectionProperty, const void* source, void* destination)
	{
		if (reflectionProperty->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
		{
			ReflectionPropertyObject* objectProperty = static_cast<ReflectionPropertyObject*>(reflectionProperty);
			if (objectProperty->HasSetMethod())
			{
				objectProperty->SetValue(destination, objectProperty->GetValue(source));
				return;
			}
		}
		reflectionProperty->Copy(source, destination); // ReflectionPropertyVariable::Copy already goes through SetValue
	}

	/// @brief Reset the property to the value it has in a default constructed root object
	/// Starting from the root keeps the defaults set by the owning type (ex: a Node anchor) instead of the ones of the nested type (a zero Vector2)
	static bool ResetToDefault(EditorReflectedProperty& editorReflectedProperty)
	{
		Vector<ReflectionPropertyObject*> path;
		const ReflectionDescriptor*       rootDescriptor = FindDefaultRoot(editorReflectedProperty, path);
		if (rootDescriptor == nullptr)
		{
			return false;
		}

		void* defaultRoot = rootDescriptor->CreateInstance();
		void* defaultOwner = defaultRoot;
		for (uint32_t index = path.Size(); index > 0; --index)
		{
			defaultOwner = path[index - 1]->GetInstance(defaultOwner);
		}

		ReflectionProperty* reflectionProperty = editorReflectedProperty.GetReflectionProperty();
		for (void* instance : editorReflectedProperty.GetInstances())
		{
			CopyThroughSetter(reflectionProperty, defaultOwner, instance);
		}

		rootDescriptor->DeleteInstance(defaultRoot);
		return true;
	}

	bool ReflectionDrawer::DrawContextMenu(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		if (ImGui::BeginPopupContextItem("PropertyContext") == true)
		{
			if (ImGui::MenuItem("Revert", nullptr, false, editorReflectedProperty.IsOverride()) == true)
			{
				ReflectionProperty* reflectionProperty = editorReflectedProperty.GetReflectionProperty();
				CopyThroughSetter(reflectionProperty, editorReflectedProperty.GetSourceInstance(), editorReflectedProperty.GetInstance());
				changed = true;
			}

			Vector<ReflectionPropertyObject*> path;
			if (ImGui::MenuItem("Reset", nullptr, false, FindDefaultRoot(editorReflectedProperty, path) != nullptr) == true)
			{
				changed |= ResetToDefault(editorReflectedProperty);
			}
			ImGui::EndPopup();
		}
		return changed;
	}
}
