#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/AnchorPresetsDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>
#include <HodEngine/UI2/Layout.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorInspectorWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	UIPrefabEditorInspectorWindow::UIPrefabEditorInspectorWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Inspector");
	}

	namespace
	{
		// Stable block ids (rather than object addresses) so a block's collapsed state survives
		// selecting another node or swapping its layout.
		enum BlockId : uintptr_t
		{
			BlockNode = 1,
			BlockLayout,
			BlockLayoutParams,
		};
	}

	/// @brief
	void UIPrefabEditorInspectorWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         node = tab->GetSelectedNode();
		if (node == nullptr)
		{
			ImGui::TextDisabled("No node selected");
			return;
		}

		bool changed = false;

		// Same compartmentalization as the scene inspector's components: one block per concern.
		// 1. The node itself
		if (DrawHelper::BeginInspectorBlock((const void*)BlockNode, ICON_MDI_VECTOR_SQUARE, "Node"))
		{
			EditorReflectedObject nodeObject(node, &node->GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(nodeObject);
		}
		DrawHelper::EndInspectorBlock();

		// 2. How the node arranges its own children
		changed |= DrawLayoutBlock(node);

		// 3. How the node is placed within its parent
		ui2::LayoutParams* layoutParams = node->GetLayoutParams();
		if (layoutParams != nullptr)
		{
			String layoutParamsTitle = String("Layout Params: ") + layoutParams->GetReflectionDescriptorV().GetDisplayName();
			if (DrawHelper::BeginInspectorBlock((const void*)BlockLayoutParams, ICON_MDI_ANCHOR, layoutParamsTitle.CStr()))
			{
				if (ui2::AnchoredLayoutParams* anchoredLayoutParams = ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams))
				{
					Vector2 anchorMin = anchoredLayoutParams->GetAnchorMin();
					Vector2 anchorMax = anchoredLayoutParams->GetAnchorMax();
					Vector2 pivot = anchoredLayoutParams->GetPivot();
					if (AnchorPresetsDrawer::Draw(ImVec2(100.0f, 100.0f), anchorMin, anchorMax, pivot))
					{
						anchoredLayoutParams->SetAnchorMin(anchorMin);
						anchoredLayoutParams->SetAnchorMax(anchorMax);
						anchoredLayoutParams->SetPivot(pivot);
						changed = true;
					}
				}

				EditorReflectedObject layoutParamsObject(layoutParams, &layoutParams->GetReflectionDescriptorV(), nullptr, this);
				changed |= PropertyDrawer::DrawDescriptor(layoutParamsObject);
			}
			DrawHelper::EndInspectorBlock();
		}

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}

	/// @brief Layout attached to `node` (how its own children are arranged). Shown as a block when
	/// one is attached — its type can be changed from the combo (transform into another layout) and
	/// it can be removed from the header's close button — otherwise an "Add Layout" button is shown.
	/// Node::SetLayout takes care of converting the children's LayoutParams to the new layout's type.
	/// @return true if the node's layout was added, replaced, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawLayoutBlock(ui2::Node* node)
	{
		bool                  changed = false;
		ReflectionDescriptor* newLayoutDescriptor = nullptr;
		bool                  removeLayout = false;

		const auto&  layoutDescriptors = ui2::LayoutFactory::GetInstance()->GetAllDescriptors();
		ui2::Layout* layout = node->GetLayout();

		if (layout != nullptr)
		{
			ReflectionDescriptor& currentDescriptor = layout->GetReflectionDescriptorV();

			String layoutTitle = String("Layout: ") + currentDescriptor.GetDisplayName();
			if (DrawHelper::BeginInspectorBlock((const void*)BlockLayout, ICON_MDI_VIEW_QUILT, layoutTitle.CStr(), nullptr, &removeLayout))
			{
				// Picking another type transforms the layout; picking the current one is a no-op.
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::BeginCombo("##LayoutType", currentDescriptor.GetDisplayName().CStr()))
				{
					for (const auto& descriptorPair : layoutDescriptors)
					{
						bool isCurrent = (descriptorPair.second->GetType() == currentDescriptor.GetType());
						if (ImGui::Selectable(descriptorPair.second->GetDisplayName().CStr(), isCurrent) && isCurrent == false)
						{
							newLayoutDescriptor = descriptorPair.second;
						}
					}
					ImGui::EndCombo();
				}

				EditorReflectedObject layoutObject(layout, &currentDescriptor, nullptr, this);
				changed |= PropertyDrawer::DrawDescriptor(layoutObject);
			}
			DrawHelper::EndInspectorBlock();
		}
		else
		{
			ImVec2 addLayoutButtonSize = ImGui::CalcTextSize("  Add Layout  ") + ImVec2(35.0f, 15.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addLayoutButtonSize.x * 0.5f));
			if (ImGui::Button("  Add Layout  ", addLayoutButtonSize))
			{
				ImGui::OpenPopup("UIPrefabAddLayout");
			}
			if (ImGui::BeginPopup("UIPrefabAddLayout"))
			{
				for (const auto& descriptorPair : layoutDescriptors)
				{
					if (ImGui::MenuItem(descriptorPair.second->GetDisplayName().CStr()))
					{
						newLayoutDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndPopup();
			}
			ImGui::Spacing();
		}

		// Applied once everything referencing the current layout has been drawn: SetLayout destroys it.
		if (removeLayout)
		{
			node->SetLayout(nullptr);
			changed = true;
		}
		else if (newLayoutDescriptor != nullptr)
		{
			node->SetLayout(newLayoutDescriptor->CreateInstance<ui2::Layout>());
			changed = true;
		}

		return changed;
	}
}
