#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/CustomPropertyDrawer/ZOrderCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Widgets.hpp"
#include "HodEngine/ImGui/Helper.hpp"

namespace hod::inline editor
{
	/// @brief 
	/// @param editorReflectedProperty 
	/// @return 
	bool ZOrderCustomPropertyDrawer::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		//changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		//ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		ZOrder& value = *editorReflectedProperty.GetObject<ZOrder>();
		changed |= ZOrderCustomPropertyDrawer::Draw(value);
		/*
		if (changed == true)
		{
			editorReflectedProperty.SetObject(value);
		}
		*/
		return changed;
	}

	/// @brief 
	/// @param value 
	/// @return 
	bool ZOrderCustomPropertyDrawer::Draw(ZOrder& value)
	{
		bool changed = false;
		uint16_t layer = value.GetLayer();
		int16_t internalOrder = value.GetInternalOrder();

		ImGui::PushID(&value);

		ImGui::Indent();
		ImGui::TextUnformatted("Layer");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		changed |= ImGui::DragScalar("##Layer", ImGuiDataType_U16, &layer);
		ImGui::TextUnformatted("Internal Order");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		changed |= ImGui::DragScalar("##InternalOrder", ImGuiDataType_S16, &internalOrder);
		ImGui::Unindent();

		ImGui::PopID();

		if (changed)
		{
			value.SetLayer(layer);
			value.SetInternalOrder(internalOrder);
		}

		return changed;
	}
}
