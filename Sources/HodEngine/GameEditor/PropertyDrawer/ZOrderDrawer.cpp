#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/PropertyDrawer/ZOrderDrawer.hpp"

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	/// @brief
	/// @param values
	/// @return
	bool ZOrderDrawer::OnDraw(const PropertyValues<ZOrder>& values)
	{
		uint16_t layer = values.Get().GetLayer();
		int16_t  internalOrder = values.Get().GetInternalOrder();
		float    width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, values.IsMixed([](const ZOrder& zOrder) { return zOrder.GetLayer(); }));
		ImGui::SetNextItemWidth(width);
		bool layerEdited = ImGui::DragScalar("##Layer", ImGuiDataType_U16, &layer, 1.0f, nullptr, nullptr, "Layer %u");
		ImGui::PopItemFlag();
		ImGui::SetItemTooltip("Layer: drawn over the lower ones");

		ImGui::SameLine();
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, values.IsMixed([](const ZOrder& zOrder) { return zOrder.GetInternalOrder(); }));
		ImGui::SetNextItemWidth(width);
		bool internalOrderEdited = ImGui::DragScalar("##InternalOrder", ImGuiDataType_S16, &internalOrder, 1.0f, nullptr, nullptr, "Order %d");
		ImGui::PopItemFlag();
		ImGui::SetItemTooltip("Order in the layer: drawn over the lower ones");

		if (layerEdited == false && internalOrderEdited == false)
		{
			return false;
		}

		values.Edit(
			[&](ZOrder& zOrder)
			{
				if (layerEdited)
				{
					zOrder.SetLayer(layer);
				}
				if (internalOrderEdited)
				{
					zOrder.SetInternalOrder(internalOrder);
				}
			});
		return true;
	}
}
