#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/AnchorPresetsDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include <HodEngine/Core/Reflection/EnumDescriptor.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>
#include <HodEngine/UI2/Layout.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

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
			BlockCanvas,
		};

		// Node properties that are not drawn among the node's content: the name has its own row on top, the
		// transform goes with the LayoutParams in the Placement block.
		constexpr std::string_view NameProperty = "Name";
		constexpr std::string_view GeometryProperties[] = {"Rotation", "Scale", "Origin"};

		bool IsGeometryProperty(std::string_view name)
		{
			for (std::string_view geometryProperty : GeometryProperties)
			{
				if (name == geometryProperty)
				{
					return true;
				}
			}
			return false;
		}

		// LayoutParams' WidthMode/HeightMode, drawn together on one Size Mode row (see DrawSizeModeRow)
		constexpr std::string_view WidthModeProperty = "WidthMode";
		constexpr std::string_view HeightModeProperty = "HeightMode";

		/// @brief One component of the Size Mode row: a framed axis label glued to the enum combo, same look as DrawHelper::DrawVector2's components.
		/// Right-clicking the combo opens the property's own context menu (Revert, Reset).
		/// @return true if the value was edited
		bool DrawSizeModeComponent(EditorReflectedProperty& property, const char* axisLabel, const ImVec4& axisColor, float width, bool used, const char* unusedReason)
		{
			bool changed = false;

			ImGui::PushID(property.GetReflectionProperty());
			ImGui::BeginDisabled(used == false);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, axisColor);
			FramedText(axisLabel, ImDrawFlags_RoundLeft);
			ImGui::PopStyleColor();
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1);

			// Same lookup as PropertyDrawer's enum combo (SizeMode is reflected as a UInt8)
			EnumDescriptor* enumDescriptor = static_cast<ReflectionPropertyVariable*>(property.GetReflectionProperty())->GetEnumDescriptor();
			uint8_t         value = property.GetValue<uint8_t>();
			const char*     valueLabel = "NotFound";
			for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
			{
				if (static_cast<uint8_t>(enumValue.first) == value)
				{
					valueLabel = enumValue.second.CStr();
					break;
				}
			}

			ImGui::SetNextItemWidth(width);
			// No preview text from BeginCombo: drawn centered below, as the Vector2 fields' values are
			if (ImGui::BeginCombo("##Mode", nullptr))
			{
				for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
				{
					if (ImGui::MenuItem(enumValue.second.CStr()) && static_cast<uint8_t>(enumValue.first) != value)
					{
						property.SetValue<uint8_t>(static_cast<uint8_t>(enumValue.first));
						changed = true;
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::BeginComboPreview())
			{
				const ImRect& previewRect = ImGui::GetCurrentContext()->ComboPreviewData.PreviewRect;
				ImGui::RenderTextClipped(previewRect.Min, previewRect.Max, valueLabel, nullptr, nullptr, ImVec2(0.5f, 0.5f));
				ImGui::EndComboPreview();
			}
			ImGui::EndDisabled();
			if (used == false)
			{
				ImGui::SetItemTooltip("%s", unusedReason);
			}
			changed |= PropertyDrawer::DrawContextMenu(property);
			ImGui::PopID();

			return changed;
		}

		/// @brief WidthMode and HeightMode on a single Size Mode row, as the X/Y components of a Vector2 (they go with Size, drawn the same way).
		/// @param usedX/usedY grey out an axis the mode is unused on
		/// @return true if one of them was edited
		bool DrawSizeModeRow(EditorReflectedObject& layoutParamsObject, bool usedX, bool usedY, const char* unusedReasonX, const char* unusedReasonY)
		{
			EditorReflectedProperty* widthMode = layoutParamsObject.FindProperty(WidthModeProperty);
			EditorReflectedProperty* heightMode = layoutParamsObject.FindProperty(HeightModeProperty);
			if (widthMode == nullptr || heightMode == nullptr)
			{
				return false;
			}

			// The override marker of PropertyDrawer::BeginProperty, for either axis
			if (widthMode->IsOverride() || heightMode->IsOverride())
			{
				float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y),
				                                          ImVec2(ImGui::GetWindowPos().x + 2.0f, ImGui::GetCursorScreenPos().y + height), IM_COL32(0, 170, 255, 255));
			}

			ImGui::BeginDisabled(usedX == false && usedY == false);
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Size Mode");
			ImGui::EndDisabled();
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

			// Same split as DrawHelper::DrawVector2, for the combos to line up with the Size row's fields
			float availableWidth = ImGui::GetColumnWidth();
			availableWidth -= CalculateButtonSize("X").x;
			availableWidth -= CalculateButtonSize("Y").x;
			availableWidth -= ImGui::GetStyle().ItemSpacing.x - 2;

			bool changed = DrawSizeModeComponent(*widthMode, "X", ImVec4(0.5f, 0.0f, 0.0f, 1.0f), availableWidth * 0.5f, usedX, unusedReasonX);
			ImGui::SameLine();
			changed |= DrawSizeModeComponent(*heightMode, "Y", ImVec4(0.0f, 0.5f, 0.0f, 1.0f), availableWidth * 0.5f, usedY, unusedReasonY);
			return changed;
		}
	}

	/// @brief
	void UIPrefabEditorInspectorWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         node = tab->GetSelectedNode();
		if (tab->IsCanvasSelected())
		{
			if (DrawCanvasBlock(tab->GetCanvas()))
			{
				GetOwner()->MarkAsDirty();
			}
			return;
		}
		if (node == nullptr)
		{
			ImGui::TextDisabled("No node selected");
			return;
		}

		bool                        changed = false;
		const ReflectionDescriptor* convertDescriptor = nullptr;

		EditorReflectedObject nodeObject(node, &node->GetReflectionDescriptorV(), nullptr, this);

		changed |= DrawNameRow(node, nodeObject);

		// 1. Where the node is and how big: what is looked at and edited the most
		changed |= DrawPlacementBlock(node, nodeObject);

		// 2. What the node's type adds
		changed |= DrawContentBlock(node, nodeObject, convertDescriptor);

		// 3. How the node arranges its own children: rarely added, so last
		changed |= DrawLayoutBlock(node);

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}

		// Last: the conversion deletes the node everything above was drawing.
		if (convertDescriptor != nullptr)
		{
			tab->ConvertNode(node, *convertDescriptor);
		}
	}

	/// @brief The node's icon and name, above the blocks (the name is what tells the selected node apart, as in the hierarchy).
	/// @return true if the name was edited
	bool UIPrefabEditorInspectorWindow::DrawNameRow(ui2::Node* node, EditorReflectedObject& nodeObject)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(DrawHelper::GetTypeIcon(node->GetReflectionDescriptorV(), ICON_MDI_VECTOR_SQUARE));
		// Read-only: assigned once, on the first save, and never edited (see Node::GetLocalId). Only of use to debug.
		if (node->GetLocalId() != 0)
		{
			ImGui::SetItemTooltip("Local Id: %llu", (unsigned long long)node->GetLocalId());
		}
		else
		{
			ImGui::SetItemTooltip("Local Id: assigned on save");
		}
		ImGui::SameLine();

		bool                     changed = false;
		EditorReflectedProperty* nameProperty = nodeObject.FindProperty(NameProperty);
		if (nameProperty != nullptr)
		{
			changed = PropertyDrawer::DrawProperty(*nameProperty, true);
		}
		ImGui::Spacing();
		return changed;
	}

	/// @brief How the node is placed within its parent: its LayoutParams, whose type depends on the parent's
	/// layout (anchors without one), then the node's own geometry.
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawPlacementBlock(ui2::Node* node, EditorReflectedObject& nodeObject)
	{
		bool changed = false;

		// The title says where the fields come from: they change with the parent's layout, e.g. when the node is moved.
		ui2::Node*         parent = node->GetParent();
		ui2::LayoutParams* layoutParams = node->GetLayoutParams();
		String             title = "Placement";
		if (parent != nullptr && parent->GetLayout() != nullptr)
		{
			const String& parentName = parent->GetName().Empty() ? parent->GetReflectionDescriptorV().GetDisplayName() : parent->GetName();
			title += String(" - ") + parent->GetLayout()->GetReflectionDescriptorV().GetDisplayName() + " of " + parentName;
		}
		else if (layoutParams != nullptr)
		{
			title += String(" - ") + layoutParams->GetReflectionDescriptorV().GetDisplayName();
		}

		if (DrawHelper::BeginInspectorBlock((const void*)BlockLayoutParams, ICON_MDI_ANCHOR, title.CStr()))
		{
			if (ui2::AnchoredLayoutParams* anchoredLayoutParams = ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams))
			{
				changed |= DrawAnchoredPlacement(node, anchoredLayoutParams);
			}
			else if (layoutParams != nullptr)
			{
				// In declaration order, WidthMode/HeightMode merged into one row where WidthMode is
				EditorReflectedObject layoutParamsObject(layoutParams, &layoutParams->GetReflectionDescriptorV(), nullptr, this);
				for (EditorReflectedProperty* property : layoutParamsObject.GetProperties())
				{
					std::string_view name = property->GetReflectionProperty()->GetName();
					if (name == WidthModeProperty)
					{
						changed |= DrawSizeModeRow(layoutParamsObject, true, true, "", "");
					}
					else if (name != HeightModeProperty)
					{
						changed |= PropertyDrawer::DrawProperty(*property);
					}
				}
			}

			// In GeometryProperties' order, not the declaration's one.
			for (std::string_view geometryProperty : GeometryProperties)
			{
				if (EditorReflectedProperty* property = nodeObject.FindProperty(geometryProperty))
				{
					changed |= PropertyDrawer::DrawProperty(*property);
				}
			}
		}
		DrawHelper::EndInspectorBlock();

		return changed;
	}

	/// @brief AnchoredLayoutParams' fields, grouped by what they do rather than in declaration order:
	/// 1. the anchors: the preset picker, then the fields it sets (AnchorMin/Max, Pivot) right below. Editing
	///    any of them keeps the node where it is (AnchoredLayoutParams::PlaceRect), as picking a preset does.
	/// 2. the rect relative to the anchors: Offset, then the size (mode + Size, a delta on a stretched axis),
	///    then its bounds.
	/// A field unused with the current anchors is greyed out (the X Size Mode on a stretched width...).
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawAnchoredPlacement(ui2::Node* node, ui2::AnchoredLayoutParams* layoutParams)
	{
		bool                  changed = false;
		EditorReflectedObject layoutParamsObject(layoutParams, &layoutParams->GetReflectionDescriptorV(), nullptr, this);

		auto drawProperty = [&](std::string_view name, bool used, const char* unusedReason) -> bool
		{
			EditorReflectedProperty* property = layoutParamsObject.FindProperty(name);
			if (property == nullptr)
			{
				return false;
			}
			ImGui::BeginDisabled(used == false);
			bool propertyChanged = PropertyDrawer::DrawProperty(*property);
			ImGui::EndDisabled();
			if (used == false)
			{
				ImGui::SetItemTooltip("%s", unusedReason);
			}
			return propertyChanged;
		};

		// 1. Anchors. Where the node is before any change, to keep it there.
		ui2::Node* parent = node->GetParent();
		Vector2    parentSize = parent != nullptr ? parent->GetSize() : Vector2::Zero;
		Vector2    center = node->GetPosition();
		Vector2    size = node->GetSize();

		Vector2 anchorMin = layoutParams->GetAnchorMin();
		Vector2 anchorMax = layoutParams->GetAnchorMax();
		Vector2 pivot = layoutParams->GetPivot();
		bool    anchorsChanged = false;

		// A row like the properties' ones (see PropertyDrawer::BeginProperty): its label, then the fixed size picker in the value column
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Anchor Preset");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		if (AnchorPresetsDrawer::Draw(ImVec2(100.0f, 100.0f), anchorMin, anchorMax, pivot))
		{
			layoutParams->SetAnchorMin(anchorMin);
			layoutParams->SetAnchorMax(anchorMax);
			layoutParams->SetPivot(pivot);
			anchorsChanged = true;
		}
		anchorsChanged |= drawProperty("AnchorMin", true, "");
		anchorsChanged |= drawProperty("AnchorMax", true, "");
		anchorsChanged |= drawProperty("Pivot", true, "");

		if (anchorsChanged)
		{
			layoutParams->PlaceRect(parentSize, center, size);
			changed = true;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// 2. The rect relative to the anchors
		bool stretchedX = layoutParams->IsStretchedX();
		bool stretchedY = layoutParams->IsStretchedY();
		bool fixedX = stretchedX == false && layoutParams->GetWidthMode() == ui2::LayoutParams::SizeMode::Fixed;
		bool fixedY = stretchedY == false && layoutParams->GetHeightMode() == ui2::LayoutParams::SizeMode::Fixed;

		changed |= drawProperty("Offset", true, "");
		changed |= DrawSizeModeRow(layoutParamsObject, stretchedX == false, stretchedY == false, "Unused: the width is stretched, the anchors and the size delta give it",
		                           "Unused: the height is stretched, the anchors and the size delta give it");

		// Size, by hand rather than reflected to tell its components apart: a delta added to the anchors' span on a stretched axis (dX/dY), the node's own size on the others
		if (EditorReflectedProperty* sizeProperty = layoutParamsObject.FindProperty("Size"))
		{
			// A Content axis anchored on a point takes its size from the content: that component is unused
			bool usedX = fixedX || stretchedX;
			bool usedY = fixedY || stretchedY;
			bool used = usedX || usedY;
			ImGui::PushID(sizeProperty->GetReflectionProperty());
			ImGui::BeginDisabled(used == false);
			changed |= PropertyDrawer::BeginProperty(*sizeProperty);
			if (used == false)
			{
				ImGui::SetItemTooltip("Unused: no axis is Fixed or stretched (sized from the content)");
			}
			else if (stretchedX || stretchedY)
			{
				ImGui::SetItemTooltip("dX/dY: added to the anchors' span on a stretched axis, around the pivot");
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
			Vector2 sizeValue = *sizeProperty->GetObject<Vector2>();
			if (DrawHelper::DrawVector2(sizeValue, stretchedX ? "dX" : "X", stretchedY ? "dY" : "Y", usedX, usedY))
			{
				sizeProperty->SetObject(sizeValue);
				changed = true;
			}
			ImGui::EndDisabled();
			ImGui::PopID();
		}

		changed |= drawProperty("MinSize", true, "");
		changed |= drawProperty("MaxSize", true, "");

		return changed;
	}

	/// @brief What the node's type adds (Text, Image...). Titled after that type; converting the node to another
	/// type is an action of the header (see UIPrefabEditorTab::ConvertNode), not a field. Only the header for a
	/// plain Node, which adds nothing.
	/// @param convertDescriptor set to the type picked from Convert To, if any
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawContentBlock(ui2::Node* node, EditorReflectedObject& nodeObject, const ReflectionDescriptor*& convertDescriptor)
	{
		bool changed = false;

		ReflectionDescriptor& currentDescriptor = node->GetReflectionDescriptorV();
		bool                  convertRequested = false;
		bool blockOpened = DrawHelper::BeginInspectorBlock((const void*)BlockNode, DrawHelper::GetTypeIcon(currentDescriptor, ICON_MDI_VECTOR_SQUARE),
		                                                   currentDescriptor.GetDisplayName().CStr(), nullptr, nullptr, ICON_MDI_SWAP_HORIZONTAL, "Convert To", &convertRequested);
		if (convertRequested)
		{
			ImGui::OpenPopup("UIPrefabConvertNode");
		}
		if (ImGui::BeginPopup("UIPrefabConvertNode"))
		{
			ImGui::TextDisabled("Convert To");
			ImGui::Separator();
			for (const auto& descriptorPair : ui2::NodeFactory::GetInstance()->GetAllDescriptors())
			{
				bool   isCurrent = (descriptorPair.second->GetType() == currentDescriptor.GetType());
				String typeLabel = String(DrawHelper::GetTypeIcon(*descriptorPair.second)) + " " + descriptorPair.second->GetDisplayName();
				if (ImGui::MenuItem(typeLabel.CStr(), nullptr, isCurrent, isCurrent == false))
				{
					convertDescriptor = descriptorPair.second;
				}
			}
			ImGui::EndPopup();
		}
		if (blockOpened)
		{
			for (EditorReflectedProperty* property : nodeObject.GetProperties())
			{
				std::string_view name = property->GetReflectionProperty()->GetName();
				if (name != NameProperty && IsGeometryProperty(name) == false)
				{
					changed |= PropertyDrawer::DrawProperty(*property);
				}
			}
		}
		DrawHelper::EndInspectorBlock();

		return changed;
	}

	/// @brief The canvas' scaler: design resolution and how it adapts to the display's ratio.
	/// @return true if the scaler was edited
	bool UIPrefabEditorInspectorWindow::DrawCanvasBlock(ui2::Canvas& canvas)
	{
		bool changed = false;

		if (DrawHelper::BeginInspectorBlock((const void*)BlockCanvas, ICON_MDI_MONITOR_SCREENSHOT, "Canvas Scaler"))
		{
			_canvasScaler = canvas.GetScaler();

			EditorReflectedObject scalerObject(&_canvasScaler, &ui2::CanvasScaler::GetReflectionDescriptor(), nullptr, this);
			if (PropertyDrawer::DrawDescriptor(scalerObject))
			{
				canvas.SetScaler(_canvasScaler);
				changed = true;
			}
		}
		DrawHelper::EndInspectorBlock();

		return changed;
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

			String layoutTitle = String("Children Layout: ") + currentDescriptor.GetDisplayName();
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
			ImVec2 addLayoutButtonSize = ImGui::CalcTextSize("  Add Children Layout  ") + ImVec2(35.0f, 15.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addLayoutButtonSize.x * 0.5f));
			if (ImGui::Button("  Add Children Layout  ", addLayoutButtonSize))
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
