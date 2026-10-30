#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/UI2Editor/AnchorPresetsDrawer.hpp"
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
#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/Interaction.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorInspectorWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorInspectorWindow::CanvasScalerFields, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CanvasScalerFields::_overrideProjectSettings, "OverrideProjectSettings");
		AddPropertyT(reflectionDescriptor, &CanvasScalerFields::_designResolution, "DesignResolution");
		AddPropertyT(reflectionDescriptor, &CanvasScalerFields::_mode, "Mode");
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
			BlockInteraction,
		};

		// Node properties that are not drawn among the node's content: the name has its own row on top, the
		// transform goes with the LayoutParams in the Placement block.
		constexpr std::string_view NameProperty = "Name";

		// Node property drawn greyed out when the node has an Interaction, which catches the pointer anyway
		constexpr std::string_view PointerTargetProperty = "PointerTarget";
		constexpr std::string_view GeometryProperties[] = {"Rotation", "Scale", "Pivot"};

		// CanvasScalerFields' row toggling the other ones (see DrawCanvasBlock)
		constexpr std::string_view OverrideProjectSettingsProperty = "OverrideProjectSettings";

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

		// Declared by LayoutParams itself, hence shared by every LayoutParams type: drawn in their own section (see DrawSizeFields)
		constexpr std::string_view CommonLayoutParamsProperties[] = {WidthModeProperty, HeightModeProperty, "Size", "MinSize", "MaxSize"};

		bool IsCommonLayoutParamsProperty(std::string_view name)
		{
			for (std::string_view commonProperty : CommonLayoutParamsProperties)
			{
				if (name == commonProperty)
				{
					return true;
				}
			}
			return false;
		}

		/// @brief A reflected property of the LayoutParams, greyed out (with unusedReason as tooltip) when it is unused with the current settings.
		/// @return true if it was edited
		bool DrawLayoutParamsProperty(EditorReflectedObject& layoutParamsObject, std::string_view name, bool used = true, const char* unusedReason = "")
		{
			EditorReflectedProperty* property = layoutParamsObject.FindProperty(name);
			if (property == nullptr)
			{
				return false;
			}
			ImGui::BeginDisabled(used == false);
			bool changed = PropertyDrawer::DrawProperty(*property);
			ImGui::EndDisabled();
			if (used == false)
			{
				ImGui::SetItemTooltip("%s", unusedReason);
			}
			return changed;
		}

		void DrawSectionSeparator()
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

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
		if (tab->IsPlaying())
		{
			DrawPlayingState(tab->GetDisplayedCanvas(), tab->GetInteractionLog());
			return;
		}

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

		// 3. How the node reacts to the pointer
		changed |= DrawInteractionBlock(node);

		// 4. How the node arranges its own children: rarely added, so last
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

	/// @brief How the node is placed within its parent, in three sections:
	/// 1. the fields of its LayoutParams' type, which depends on the parent's layout (anchors without one, see DrawAnchoredFields)
	/// 2. the size fields every LayoutParams shares (see DrawSizeFields)
	/// 3. the node's own transform (Rotation, Scale, Pivot), purely visual
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
			if (layoutParams != nullptr)
			{
				EditorReflectedObject layoutParamsObject(layoutParams, &layoutParams->GetReflectionDescriptorV(), nullptr, this);

				// 1. The type's own fields
				if (ui2::AnchoredLayoutParams* anchoredLayoutParams = ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams))
				{
					changed |= DrawAnchoredFields(node, anchoredLayoutParams, layoutParamsObject);
					DrawSectionSeparator();
				}
				else
				{
					// In declaration order
					bool anyOwnField = false;
					for (EditorReflectedProperty* property : layoutParamsObject.GetProperties())
					{
						if (IsCommonLayoutParamsProperty(property->GetReflectionProperty()->GetName()) == false)
						{
							changed |= PropertyDrawer::DrawProperty(*property);
							anyOwnField = true;
						}
					}
					if (anyOwnField)
					{
						DrawSectionSeparator();
					}
				}

				// 2. The shared size fields
				changed |= DrawSizeFields(layoutParams, layoutParamsObject);
				DrawSectionSeparator();
			}

			// 3. The node's own transform. In GeometryProperties' order, not the declaration's one.
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

	/// @brief AnchoredLayoutParams' own fields, grouped by what they do rather than in declaration order: the anchor
	/// preset picker, then the fields it sets (AnchorMin/Max, Alignment) right below, then Offset. Editing the anchors
	/// keeps the node where it is (AnchoredLayoutParams::PlaceRect), as picking a preset does.
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawAnchoredFields(ui2::Node* node, ui2::AnchoredLayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject)
	{
		bool changed = false;

		// Where the node is before any change, to keep it there.
		ui2::Node* parent = node->GetParent();
		Vector2    parentSize = parent != nullptr ? parent->GetSize() : Vector2::Zero;
		Vector2    center = node->GetPosition();
		Vector2    size = node->GetSize();

		Vector2 anchorMin = layoutParams->GetAnchorMin();
		Vector2 anchorMax = layoutParams->GetAnchorMax();
		Vector2 alignment = layoutParams->GetAlignment();
		bool    anchorsChanged = false;

		// A row like the properties' ones (see PropertyDrawer::BeginProperty): its label, then the fixed size picker in the value column
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Anchor Preset");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		if (AnchorPresetsDrawer::Draw(ImVec2(100.0f, 100.0f), anchorMin, anchorMax, alignment))
		{
			layoutParams->SetAnchorMin(anchorMin);
			layoutParams->SetAnchorMax(anchorMax);
			layoutParams->SetAlignment(alignment);
			anchorsChanged = true;
		}
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "AnchorMin");
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "AnchorMax");
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "Alignment");

		if (anchorsChanged)
		{
			layoutParams->PlaceRect(parentSize, center, size);
			changed = true;
		}

		changed |= DrawLayoutParamsProperty(layoutParamsObject, "Offset");

		return changed;
	}

	/// @brief The size fields every LayoutParams shares: the mode (Fixed/Content), Size, then its bounds. Under AnchoredLayoutParams,
	/// a field unused with the current anchors is greyed out (the X Size Mode on a stretched width...), and Size is a delta
	/// (dX/dY) on a stretched axis.
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawSizeFields(ui2::LayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject)
	{
		bool changed = false;

		ui2::AnchoredLayoutParams* anchoredLayoutParams = ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams);
		bool                       stretchedX = anchoredLayoutParams != nullptr && anchoredLayoutParams->IsStretchedX();
		bool                       stretchedY = anchoredLayoutParams != nullptr && anchoredLayoutParams->IsStretchedY();
		bool                       fixedX = stretchedX == false && layoutParams->GetWidthMode() == ui2::LayoutParams::SizeMode::Fixed;
		bool                       fixedY = stretchedY == false && layoutParams->GetHeightMode() == ui2::LayoutParams::SizeMode::Fixed;

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
				ImGui::SetItemTooltip("dX/dY: added to the anchors' span on a stretched axis, around the alignment point");
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

		changed |= DrawLayoutParamsProperty(layoutParamsObject, "MinSize");
		changed |= DrawLayoutParamsProperty(layoutParamsObject, "MaxSize");

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
				if (name == PointerTargetProperty)
				{
					bool hasInteraction = (node->GetInteraction() != nullptr);
					ImGui::BeginDisabled(hasInteraction);
					changed |= PropertyDrawer::DrawProperty(*property);
					ImGui::EndDisabled();
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
					{
						ImGui::SetTooltip(hasInteraction ? "Always on with an Interaction"
						                                 : "Whether the pointer stops on this node: what is under it doesn't get it");
					}
				}
				else if (name != NameProperty && IsGeometryProperty(name) == false)
				{
					changed |= PropertyDrawer::DrawProperty(*property);
				}
			}
		}
		DrawHelper::EndInspectorBlock();

		return changed;
	}

	/// @brief The canvas' scaler: design resolution and how it adapts to the display's ratio, the project's
	/// (UISettings, shown greyed out) unless the canvas overrides it.
	/// @return true if the scaling was edited
	bool UIPrefabEditorInspectorWindow::DrawCanvasBlock(ui2::Canvas& canvas)
	{
		bool changed = false;

		if (DrawHelper::BeginInspectorBlock((const void*)BlockCanvas, ICON_MDI_MONITOR_SCREENSHOT, "Canvas Scaler"))
		{
			const ui2::CanvasScaler& effectiveScaler = canvas.GetEffectiveScaler();
			_canvasScalerFields._overrideProjectSettings = canvas.GetOverrideScaler();
			_canvasScalerFields._designResolution = effectiveScaler._designResolution;
			_canvasScalerFields._mode = effectiveScaler._mode;

			EditorReflectedObject fieldsObject(&_canvasScalerFields, &CanvasScalerFields::GetReflectionDescriptor(), nullptr, this);
			for (EditorReflectedProperty* property : fieldsObject.GetProperties())
			{
				if (property->GetReflectionProperty()->GetName() == OverrideProjectSettingsProperty)
				{
					if (PropertyDrawer::DrawProperty(*property))
					{
						if (_canvasScalerFields._overrideProjectSettings)
						{
							canvas.SetScaler(effectiveScaler); // Starts from the project's values
						}
						canvas.SetOverrideScaler(_canvasScalerFields._overrideProjectSettings);
						changed = true;
					}
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
					{
						ImGui::SetTooltip("Off: the canvas follows the project's UI settings (Edit > Project Settings)");
					}
				}
				else
				{
					ImGui::BeginDisabled(canvas.GetOverrideScaler() == false);
					if (PropertyDrawer::DrawProperty(*property))
					{
						ui2::CanvasScaler scaler;
						scaler._designResolution = _canvasScalerFields._designResolution;
						scaler._mode = _canvasScalerFields._mode;
						canvas.SetScaler(scaler);
						changed = true;
					}
					ImGui::EndDisabled();
				}
			}
		}
		DrawHelper::EndInspectorBlock();

		return changed;
	}

	/// @brief Interaction attached to `node` (how it reacts to the pointer). Shown as a block when one is attached, removable from
	/// the header's close button, otherwise an "Add Interaction" button is shown.
	/// @return true if the node's interaction was added, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawInteractionBlock(ui2::Node* node)
	{
		bool                  changed = false;
		ReflectionDescriptor* newInteractionDescriptor = nullptr;
		bool                  removeInteraction = false;

		ui2::Interaction* interaction = node->GetInteraction();
		if (interaction != nullptr)
		{
			ReflectionDescriptor& currentDescriptor = interaction->GetReflectionDescriptorV();

			String title = String("Interaction: ") + currentDescriptor.GetDisplayName();
			if (DrawHelper::BeginInspectorBlock((const void*)BlockInteraction, DrawHelper::GetTypeIcon(currentDescriptor, ICON_MDI_GESTURE_TAP), title.CStr(), nullptr,
			                                    &removeInteraction))
			{
				EditorReflectedObject interactionObject(interaction, &currentDescriptor, nullptr, this);
				changed |= PropertyDrawer::DrawDescriptor(interactionObject);

				if (ui2::Button* button = ui2::Interaction::Cast<ui2::Button>(interaction))
				{
					changed |= DrawStateOverrides(node, *button);
				}
			}
			DrawHelper::EndInspectorBlock();
		}
		else
		{
			ImVec2 addButtonSize = ImGui::CalcTextSize("  Add Interaction  ") + ImVec2(35.0f, 15.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addButtonSize.x * 0.5f));
			if (ImGui::Button("  Add Interaction  ", addButtonSize))
			{
				ImGui::OpenPopup("UIPrefabAddInteraction");
			}
			if (ImGui::BeginPopup("UIPrefabAddInteraction"))
			{
				for (const auto& descriptorPair : ui2::InteractionFactory::GetInstance()->GetAllDescriptors())
				{
					String label = String(DrawHelper::GetTypeIcon(*descriptorPair.second)) + " " + descriptorPair.second->GetDisplayName();
					if (ImGui::MenuItem(label.CStr()))
					{
						newInteractionDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndPopup();
			}
			ImGui::Spacing();
		}

		// Applied once everything referencing the current interaction has been drawn: SetInteraction destroys it.
		if (removeInteraction)
		{
			node->SetInteraction(nullptr);
			changed = true;
		}
		else if (newInteractionDescriptor != nullptr)
		{
			node->SetInteraction(newInteractionDescriptor->CreateInstance<ui2::Interaction>());
			changed = true;
		}

		return changed;
	}

	namespace
	{
		/// @brief A node of the prefab, as listed in the override target picker: its icon and name.
		String GetNodeLabel(const ui2::Node& node)
		{
			String name = node.GetName().Empty() ? String("(unnamed)") : node.GetName();
			return String(DrawHelper::GetTypeIcon(node.GetReflectionDescriptorV(), ICON_MDI_VECTOR_SQUARE)) + " " + name;
		}

		/// @brief A value as serialized, on one line: objects and arrays as their values between parentheses.
		String FormatValue(const DocumentNode& node)
		{
			char buffer[64] = {};
			switch (node.GetType())
			{
				case DocumentNode::Type::Object:
				case DocumentNode::Type::Array:
				{
					String text = "(";
					for (const DocumentNode* child = node.GetFirstChild(); child != nullptr; child = child->GetNextSibling())
					{
						if (child != node.GetFirstChild())
						{
							text += ", ";
						}
						text += FormatValue(*child);
					}
					return text + ")";
				}
				case DocumentNode::Type::Bool: return node.GetBool() ? "true" : "false";
				case DocumentNode::Type::Int8: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt8()); break;
				case DocumentNode::Type::Int16: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt16()); break;
				case DocumentNode::Type::Int32: std::snprintf(buffer, sizeof(buffer), "%d", (int)node.GetInt32()); break;
				case DocumentNode::Type::Int64: std::snprintf(buffer, sizeof(buffer), "%lld", (long long)node.GetInt64()); break;
				case DocumentNode::Type::UInt8: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt8()); break;
				case DocumentNode::Type::UInt16: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt16()); break;
				case DocumentNode::Type::UInt32: std::snprintf(buffer, sizeof(buffer), "%u", (unsigned)node.GetUInt32()); break;
				case DocumentNode::Type::UInt64: std::snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)node.GetUInt64()); break;
				case DocumentNode::Type::Float32: std::snprintf(buffer, sizeof(buffer), "%.3g", (double)node.GetFloat32()); break;
				case DocumentNode::Type::Float64: std::snprintf(buffer, sizeof(buffer), "%.3g", node.GetFloat64()); break;
				case DocumentNode::Type::String: return String("\"") + node.GetString() + "\"";
			}
			return buffer;
		}
	}

	/// @brief A Button's look per state: for Hovered, Pressed and Disabled, the list of property overrides applied in that state
	/// (Normal is the prefab as authored), each a target node, one of its properties and the value it takes. Seen in play mode.
	/// Each state can be recorded (see UIPrefabEditorTab::StartRecording): its overrides then follow the edits of the nodes, and are
	/// only removable here (which puts the property back). The other states can't be edited meanwhile.
	/// @return true if an override was added, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawStateOverrides(ui2::Node* node, ui2::Button& button)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		bool               changed = false;

		struct StateEntry
		{
			ui2::Button::State _state;
			const char*        _label;
		};
		constexpr StateEntry states[] = {
			{ui2::Button::State::Hovered, "Hovered"},
			{ui2::Button::State::Pressed, "Pressed"},
			{ui2::Button::State::Disabled, "Disabled"},
		};

		for (const StateEntry& stateEntry : states)
		{
			ImGui::PushID(stateEntry._label);

			bool recording = tab->IsRecording(node, stateEntry._state);
			bool otherRecorded = tab->IsRecording() && recording == false;

			ImGui::BeginDisabled(otherRecorded);

			// One framed block per state (red framed while recorded): its name followed by its record button, then its overrides
			ImGui::PushStyleColor(ImGuiCol_Border, recording ? ImVec4(1.0f, 80.0f / 255.0f, 80.0f / 255.0f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Border));
			bool blockVisible = ImGui::BeginChild("State", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders);
			ImGui::PopStyleColor();
			if (blockVisible == false)
			{
				ImGui::EndChild();
				ImGui::EndDisabled();
				ImGui::PopID();
				continue;
			}

			ImGui::AlignTextToFramePadding();
			if (recording)
			{
				ImGui::TextColored(ImVec4(1.0f, 80.0f / 255.0f, 80.0f / 255.0f, 1.0f), "%s", stateEntry._label);
			}
			else
			{
				ImGui::TextUnformatted(stateEntry._label);
			}
			ImGui::SameLine();
			if (recording)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
			}
			if (ImGui::SmallButton(recording ? ICON_MDI_STOP : ICON_MDI_RECORD))
			{
				if (recording)
				{
					tab->StopRecording();
				}
				else
				{
					tab->StartRecording(node, stateEntry._state);
				}
			}
			if (recording)
			{
				ImGui::PopStyleColor();
			}
			ImGui::SetItemTooltip(recording ? "Stop recording, back to the prefab as authored"
			                                : "Record: show this state, and record the properties edited on any node (inspector) as its overrides");
			ImGui::Separator();

			Vector<ui2::PropertyOverride>& overrides = *button.GetOverrides(stateEntry._state);
			int32_t                        removeIndex = -1;
			for (uint32_t index = 0; index < overrides.Size(); ++index)
			{
				ImGui::PushID((int)index);
				bool removeRequested = false;
				DrawPropertyOverride(overrides[index], recording, removeRequested);
				if (removeRequested)
				{
					removeIndex = (int32_t)index;
				}
				ImGui::PopID();
			}

			if (removeIndex >= 0)
			{
				if (recording)
				{
					// The recording drops the override once the property is back to its authored value
					tab->RevertRecordedProperty(overrides[(uint32_t)removeIndex]._target, overrides[(uint32_t)removeIndex]._property);
				}
				else
				{
					overrides.Erase((uint32_t)removeIndex);
					changed = true;
				}
			}

			if (overrides.Empty())
			{
				ImGui::TextDisabled("No override");
			}

			ImGui::EndChild();
			ImGui::EndDisabled();
			ImGui::PopID();
		}

		return changed;
	}

	/// @brief One override, read-only (edited by recording its state), on one line: the target node and its property, then the
	/// value (cut to the line, whole in the tooltip).
	/// @param propertyOverride
	/// @param recording whether its state is being recorded (its remove button then puts the property back)
	/// @param removeRequested set when its remove button is clicked
	void UIPrefabEditorInspectorWindow::DrawPropertyOverride(const ui2::PropertyOverride& propertyOverride, bool recording, bool& removeRequested)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         target = tab->GetCanvas().GetRootNode()->FindByLocalId(propertyOverride._target);

		ui2::PropertyValue value;
		bool               resolved = (target != nullptr && propertyOverride.GetValue(*target, value));

		float removeWidth = ImGui::CalcTextSize(ICON_MDI_CLOSE).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float lineStart = ImGui::GetCursorPosX();

		String targetLabel = (target != nullptr) ? GetNodeLabel(*target) : String(ICON_MDI_ALERT " Missing node");
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(targetLabel.CStr());
		ImGui::SameLine();
		ImGui::TextDisabled(ICON_MDI_CHEVRON_RIGHT);
		ImGui::SameLine();
		if (target != nullptr && resolved == false)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " %s", propertyOverride._property.CStr());
			ImGui::SetItemTooltip("The node has no such property anymore (or of another type): ignored");
		}
		else
		{
			ImGui::TextUnformatted(propertyOverride._property.CStr());
		}

		float removeX = lineStart + Max(ImGui::GetContentRegionAvail().x - removeWidth, 0.0f); // right-aligned

		// The value between the property and the remove button, clipped there
		ImGui::SameLine();
		String valueText = FormatValue(propertyOverride._value.GetRootNode());
		ImVec2 valuePos = ImGui::GetCursorScreenPos();
		float  valueWidth = Max(removeX - ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x, 0.0f);
		ImGui::PushClipRect(valuePos, ImVec2(valuePos.x + valueWidth, valuePos.y + ImGui::GetFrameHeight()), true);
		ImGui::TextDisabled("%s", valueText.CStr());
		ImGui::PopClipRect();
		if (ImGui::IsMouseHoveringRect(valuePos, ImVec2(valuePos.x + valueWidth, valuePos.y + ImGui::GetFrameHeight())))
		{
			ImGui::SetTooltip("%s", valueText.CStr());
		}

		ImGui::SameLine(removeX);
		if (ImGui::Button(ICON_MDI_CLOSE))
		{
			removeRequested = true;
		}
		ImGui::SetItemTooltip(recording ? "Remove (puts the property back to its authored value)" : "Remove");
	}

	/// @brief Play mode's state, in place of the selection (editing is suspended): the node whose Interaction gets the pointer
	/// and the one holding the press, with their Button state, then the actions emitted, most recent first.
	/// @param canvas the play mode's canvas
	/// @param log
	void UIPrefabEditorInspectorWindow::DrawPlayingState(ui2::Canvas& canvas, const Vector<UIPrefabEditorTab::InteractionLogEntry>& log)
	{
		ImGui::TextDisabled("Editing suspended while playing");
		ImGui::Spacing();

		auto drawNodeRow = [](const char* label, ui2::Node* node)
		{
			ImGui::TextUnformatted(label);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
			if (node == nullptr)
			{
				ImGui::TextDisabled("None");
				return;
			}

			String text = String(DrawHelper::GetTypeIcon(node->GetReflectionDescriptorV(), ICON_MDI_VECTOR_SQUARE)) + " " + node->GetName();
			if (const ui2::Button* button = node->GetInteraction<ui2::Button>())
			{
				constexpr const char* stateNames[] = {"Normal", "Hovered", "Pressed", "Disabled"};
				text += String(" (") + stateNames[(uint8_t)button->GetState()] + ")";
			}
			ImGui::TextUnformatted(text.CStr());
		};

		drawNodeRow("Hovered", canvas.GetHoveredNode());
		drawNodeRow("Pressed", canvas.GetPressedNode());

		ImGui::Spacing();
		ImGui::SeparatorText("Actions");
		if (log.Empty())
		{
			ImGui::TextDisabled("No action emitted yet");
			return;
		}
		for (uint32_t index = log.Size(); index > 0; --index)
		{
			const UIPrefabEditorTab::InteractionLogEntry& entry = log[index - 1];
			ImGui::Text("%s", entry._action.CStr());
			ImGui::SameLine();
			ImGui::TextDisabled("from %s, %.1fs ago", entry._nodeName.CStr(), (float)(ImGui::GetTime() - entry._time));
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

			String layoutTitle = String("Children Layout: ") + currentDescriptor.GetDisplayName();
			bool   convertRequested = false;
			bool   blockOpened = DrawHelper::BeginInspectorBlock((const void*)BlockLayout, ICON_MDI_VIEW_QUILT, layoutTitle.CStr(), nullptr, &removeLayout,
			                                                     ICON_MDI_SWAP_HORIZONTAL, "Convert To", &convertRequested);
			// Same popup as the node's Convert To: picking another type converts the layout, keeping what the two types share (see Node::ConvertLayout)
			if (convertRequested)
			{
				ImGui::OpenPopup("UIPrefabConvertLayout");
			}
			if (ImGui::BeginPopup("UIPrefabConvertLayout"))
			{
				ImGui::TextDisabled("Convert To");
				ImGui::Separator();
				for (const auto& descriptorPair : layoutDescriptors)
				{
					bool isCurrent = (descriptorPair.second->GetType() == currentDescriptor.GetType());
					if (ImGui::MenuItem(descriptorPair.second->GetDisplayName().CStr(), nullptr, isCurrent, isCurrent == false))
					{
						newLayoutDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndPopup();
			}
			if (blockOpened)
			{
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
			node->ConvertLayout(*newLayoutDescriptor);
			changed = true;
		}

		return changed;
	}
}
