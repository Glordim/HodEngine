#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/SerializedValueFormat.hpp"

#include "HodEngine/UI2Editor/AnchorPresetsDrawer.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
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
#include <HodEngine/UI2/Selectable.hpp>
#include <HodEngine/UI2/Interaction.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>
#include <HodEngine/UI2/View.hpp>
#include <HodEngine/UI2/ViewFactory.hpp>

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
			BlockView,
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
			bool            mixed = property.HasDifferentValues();
			const char*     valueLabel = mixed ? GetMixedValueLabel() : "NotFound";
			for (const std::pair<uint64_t, String>& enumValue : enumDescriptor->GetValues())
			{
				if (mixed == false && static_cast<uint8_t>(enumValue.first) == value)
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
					// Even the first node's mode, when mixed: to the other nodes
					if (ImGui::MenuItem(enumValue.second.CStr()) && (mixed || static_cast<uint8_t>(enumValue.first) != value))
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
				if (mixed)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
				}
				ImGui::RenderTextClipped(previewRect.Min, previewRect.Max, valueLabel, nullptr, nullptr, ImVec2(0.5f, 0.5f));
				if (mixed)
				{
					ImGui::PopStyleColor();
				}
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

		/// @brief The objects, as the instances of an EditorReflectedObject
		template<typename _Type_>
		Vector<void*> ToInstances(const Vector<_Type_*>& objects)
		{
			Vector<void*> instances;
			instances.Reserve(objects.Size());
			for (_Type_* object : objects)
			{
				instances.PushBack(object);
			}
			return instances;
		}

		bool IsKindOf(const ReflectionDescriptor& descriptor, const ReflectionDescriptor& base)
		{
			for (const ReflectionDescriptor* current = &descriptor; current != nullptr; current = current->GetParent())
			{
				if (current->GetType() == base.GetType()) // not the addresses: one descriptor per DLL
				{
					return true;
				}
			}
			return false;
		}

		/// @brief The most derived type all the nodes are of: the properties they share (those of a Node for a Text and an Image)
		const ReflectionDescriptor& FindCommonDescriptor(const Vector<ui2::Node*>& nodes)
		{
			const ReflectionDescriptor* common = &nodes[0]->GetReflectionDescriptorV();
			for (const ui2::Node* node : nodes)
			{
				while (common->GetParent() != nullptr && IsKindOf(node->GetReflectionDescriptorV(), *common) == false)
				{
					common = common->GetParent();
				}
			}
			return *common;
		}

		/// @brief The type of the objects (layouts, interactions...), when all of them exist and are of the same one: edited together then
		/// @return nullptr otherwise
		template<typename _Type_>
		ReflectionDescriptor* FindSharedDescriptor(const Vector<_Type_*>& objects)
		{
			ReflectionDescriptor* shared = nullptr;
			for (_Type_* object : objects)
			{
				if (object == nullptr)
				{
					return nullptr;
				}
				ReflectionDescriptor& descriptor = object->GetReflectionDescriptorV();
				if (shared == nullptr)
				{
					shared = &descriptor;
				}
				else if (descriptor.GetType() != shared->GetType())
				{
					return nullptr;
				}
			}
			return shared;
		}

		/// @brief For the nodes' Interactions, Layouts... that are neither all the same type nor all missing: nothing edited together
		void DrawDifferentTypes(const char* what)
		{
			ImGui::TextDisabled("%s %s differ: edited one node at a time", GetMixedValueLabel(), what);
			ImGui::Spacing();
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
				GetOwner()->MarkAsDirty("Edit Canvas");
			}
			return;
		}
		if (node == nullptr)
		{
			ImGui::TextDisabled("No node selected");
			return;
		}

		// The nodes edited together: the active one first, whose value a mixed value's edit starts from
		Vector<ui2::Node*> nodes;
		nodes.PushBack(node);
		for (ui2::Node* selectedNode : tab->GetSelectedNodes())
		{
			if (selectedNode != node)
			{
				nodes.PushBack(selectedNode);
			}
		}
		bool multiple = (nodes.Size() > 1);

		bool                        changed = false;
		const ReflectionDescriptor* convertDescriptor = nullptr;

		// First: unpacking the instance from there deletes its source
		if (multiple == false)
		{
			DrawPrefabInstanceRow(node);
		}

		// A node of an instance is compared to its prefab's: the overrides are marked, and revertable. Not several nodes, which have a source each.
		ui2::Node*            sourceNode = multiple ? nullptr : UIPrefabEditorTab::FindSourceNode(node);
		EditorReflectedObject nodeObject(ToInstances(nodes), &FindCommonDescriptor(nodes), sourceNode, this);

		bool readOnly = false;
		for (const ui2::Node* editedNode : nodes)
		{
			readOnly |= (UIPrefabEditorTab::CanEditProperties(editedNode) == false);
		}
		if (multiple)
		{
			ImGui::TextDisabled("%u nodes selected", nodes.Size());
			if (readOnly)
			{
				ImGui::TextDisabled("Some are in an instance held by an instance: edited in its own prefab");
			}
		}
		ImGui::BeginDisabled(readOnly);

		changed |= DrawNameRow(nodes, nodeObject);

		// 1. Where the node is and how big: what is looked at and edited the most
		changed |= DrawPlacementBlock(nodes, nodeObject, sourceNode);

		// 2. What the node's type adds
		changed |= DrawContentBlock(nodes, nodeObject, convertDescriptor);

		// 3. How the node reacts to the pointer
		changed |= DrawInteractionBlock(nodes, sourceNode);

		// 4. How the node arranges its own children: rarely added
		changed |= DrawLayoutBlock(nodes, sourceNode);

		// 5. The code driving the prefab: on its root only
		if (multiple == false)
		{
			changed |= DrawViewBlock(node);
		}

		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAsDirty(multiple ? fmt::format("Edit {} Nodes", nodes.Size()) : fmt::format("Edit {}", node->GetName()));
		}

		// Last: the conversion deletes the node everything above was drawing.
		if (convertDescriptor != nullptr)
		{
			tab->ConvertNode(node, *convertDescriptor);
		}
	}

	/// @brief The nodes' icon and name, above the blocks (the name is what tells the selected node apart, as in the hierarchy).
	/// @return true if the name was edited
	bool UIPrefabEditorInspectorWindow::DrawNameRow(const Vector<ui2::Node*>& nodes, EditorReflectedObject& nodeObject)
	{
		const ui2::Node* node = nodes[0];

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(DrawHelper::GetTypeIcon(*nodeObject.GetReflectionDescriptor(), ICON_MDI_VECTOR_SQUARE));
		// Read-only: assigned once, on the first save, and never edited (see Node::GetLocalId). Only of use to debug.
		if (nodes.Size() > 1)
		{
			ImGui::SetItemTooltip("%u nodes", nodes.Size());
		}
		else if (node->GetLocalId() != 0)
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

	/// @brief For a node of a nested prefab's instance, above the name: the prefab, with its actions for the instance's root (open
	/// the prefab, unpack the instance), what can be edited otherwise. Nothing for the other nodes.
	void UIPrefabEditorInspectorWindow::DrawPrefabInstanceRow(ui2::Node* node)
	{
		UIPrefabEditorTab*          tab = GetOwner<UIPrefabEditorTab>();
		UIPrefabEditorTab::NodeRole role = UIPrefabEditorTab::GetNodeRole(node);
		if (role == UIPrefabEditorTab::NodeRole::Authored)
		{
			return;
		}

		// The instance holding the node: its own for the root
		const ui2::Node* instanceRoot = node;
		if (role != UIPrefabEditorTab::NodeRole::InstanceRoot)
		{
			for (instanceRoot = node->GetParent(); instanceRoot != nullptr && instanceRoot->GetPrefabInstance() == nullptr; instanceRoot = instanceRoot->GetParent())
			{
			}
		}
		if (instanceRoot == nullptr)
		{
			return;
		}

		ui2::PrefabInstance*   prefabInstance = instanceRoot->GetPrefabInstance();
		std::shared_ptr<Asset> prefabAsset = AssetDatabase::GetInstance()->Find(prefabInstance->GetPrefab());
		String                 prefabName = (prefabAsset != nullptr) ? prefabAsset->GetName() : prefabInstance->GetPrefab().ToString();

		ImGui::AlignTextToFramePadding();
		ImGui::TextColored(ImVec4(0.45f, 0.75f, 1.0f, 1.0f), ICON_MDI_PACKAGE_VARIANT_CLOSED " %s", prefabName.CStr());
		if (prefabInstance->IsMissing())
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), ICON_MDI_ALERT " Missing");
			ImGui::SetItemTooltip("The prefab can't be loaded: the instance is kept as is (its overrides too) until it can");
		}

		switch (role)
		{
			case UIPrefabEditorTab::NodeRole::InstanceRoot:
			{
				ImGui::SameLine();
				ImGui::BeginDisabled(prefabAsset == nullptr);
				if (ImGui::SmallButton(ICON_MDI_OPEN_IN_NEW " Open"))
				{
					Editor::GetInstance()->OpenAsset(prefabAsset);
				}
				ImGui::EndDisabled();
				ImGui::SameLine();
				if (ImGui::SmallButton(ICON_MDI_PACKAGE_VARIANT " Unpack"))
				{
					tab->UnpackPrefabInstance(node);
				}
				ImGui::SetItemTooltip("Turn the instance into plain nodes of this prefab, no longer following its prefab");
				break;
			}
			case UIPrefabEditorTab::NodeRole::InstanceContent:
				ImGui::TextDisabled("In an instance: its edits are overrides, its structure is its prefab's");
				break;
			default: ImGui::TextDisabled("In an instance held by an instance: edited in its own prefab"); break;
		}
		ImGui::Spacing();
	}

	/// @brief How the nodes are placed within their parents, in three sections:
	/// 1. the fields of their LayoutParams' type, which depends on the parent's layout (anchors without one, see DrawAnchoredFields)
	/// 2. the size fields every LayoutParams shares (see DrawSizeFields)
	/// 3. the node's own transform (Rotation, Scale, Pivot), purely visual
	/// Nodes with LayoutParams of different types (under parents of different layouts) only share the third one.
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawPlacementBlock(const Vector<ui2::Node*>& nodes, EditorReflectedObject& nodeObject, ui2::Node* sourceNode)
	{
		bool changed = false;

		ui2::Node*                 node = nodes[0];
		Vector<ui2::LayoutParams*> layoutParamsList;
		bool                       anyLayoutParams = false;
		bool                       sameParent = true;
		for (ui2::Node* editedNode : nodes)
		{
			layoutParamsList.PushBack(editedNode->GetLayoutParams());
			anyLayoutParams |= (editedNode->GetLayoutParams() != nullptr);
			sameParent &= (editedNode->GetParent() == node->GetParent());
		}
		ReflectionDescriptor* layoutParamsDescriptor = FindSharedDescriptor(layoutParamsList);

		// The title says where the fields come from: they change with the parent's layout, e.g. when the node is moved.
		ui2::Node* parent = node->GetParent();
		String     title = "Placement";
		if (sameParent && parent != nullptr && parent->GetLayout() != nullptr)
		{
			const String& parentName = parent->GetName().Empty() ? parent->GetReflectionDescriptorV().GetDisplayName() : parent->GetName();
			title += String(" - ") + parent->GetLayout()->GetReflectionDescriptorV().GetDisplayName() + " of " + parentName;
		}
		else if (layoutParamsDescriptor != nullptr)
		{
			title += String(" - ") + layoutParamsDescriptor->GetDisplayName();
		}

		if (DrawHelper::BeginInspectorBlock((const void*)BlockLayoutParams, ICON_MDI_ANCHOR, title.CStr()))
		{
			if (layoutParamsDescriptor != nullptr)
			{
				ui2::LayoutParams* layoutParams = layoutParamsList[0];

				// An instance root's placement is the containing prefab's own: compared to nothing
				ui2::LayoutParams* sourceLayoutParams = nullptr;
				if (sourceNode != nullptr && node->GetPrefabInstance() == nullptr && sourceNode->GetLayoutParams() != nullptr &&
				    sourceNode->GetLayoutParams()->GetReflectionDescriptorV().GetType() == layoutParamsDescriptor->GetType())
				{
					sourceLayoutParams = sourceNode->GetLayoutParams();
				}
				EditorReflectedObject layoutParamsObject(ToInstances(layoutParamsList), layoutParamsDescriptor, sourceLayoutParams, this);

				// 1. The type's own fields
				if (ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams) != nullptr)
				{
					Vector<ui2::AnchoredLayoutParams*> anchoredLayoutParamsList;
					for (ui2::LayoutParams* editedLayoutParams : layoutParamsList)
					{
						anchoredLayoutParamsList.PushBack(ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(editedLayoutParams));
					}
					changed |= DrawAnchoredFields(nodes, anchoredLayoutParamsList, layoutParamsObject);
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
				changed |= DrawSizeFields(layoutParamsList, layoutParamsObject);
				DrawSectionSeparator();
			}
			else if (anyLayoutParams)
			{
				ImGui::TextDisabled("%s Placed differently (their parents' layouts differ): edited one node at a time", GetMixedValueLabel());
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
	/// keeps each node where it is (AnchoredLayoutParams::PlaceRect), as picking a preset does.
	/// @param layoutParamsList the nodes' ones, in the same order
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawAnchoredFields(const Vector<ui2::Node*>& nodes, const Vector<ui2::AnchoredLayoutParams*>& layoutParamsList,
	                                                       EditorReflectedObject& layoutParamsObject)
	{
		bool changed = false;

		// Where each node is before any change, to keep it there.
		struct Placement
		{
			Vector2 _parentSize;
			Vector2 _center;
			Vector2 _size;
		};
		Vector<Placement> placements;
		for (const ui2::Node* node : nodes)
		{
			const ui2::Node* parent = node->GetParent();
			placements.PushBack(Placement {parent != nullptr ? parent->GetSize() : Vector2::Zero, node->GetPosition(), node->GetSize()});
		}

		// The first node's preset is the one highlighted
		Vector2 anchorMin = layoutParamsList[0]->GetAnchorMin();
		Vector2 anchorMax = layoutParamsList[0]->GetAnchorMax();
		Vector2 alignment = layoutParamsList[0]->GetAlignment();
		bool    anchorsChanged = false;

		// A row like the properties' ones (see PropertyDrawer::BeginProperty): its label, then the fixed size picker in the value column
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Anchor Preset");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		if (AnchorPresetsDrawer::Draw(ImVec2(100.0f, 100.0f), anchorMin, anchorMax, alignment))
		{
			for (ui2::AnchoredLayoutParams* layoutParams : layoutParamsList)
			{
				layoutParams->SetAnchorMin(anchorMin);
				layoutParams->SetAnchorMax(anchorMax);
				layoutParams->SetAlignment(alignment);
			}
			anchorsChanged = true;
		}
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "AnchorMin");
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "AnchorMax");
		anchorsChanged |= DrawLayoutParamsProperty(layoutParamsObject, "Alignment");

		if (anchorsChanged)
		{
			for (uint32_t index = 0; index < layoutParamsList.Size(); ++index)
			{
				layoutParamsList[index]->PlaceRect(placements[index]._parentSize, placements[index]._center, placements[index]._size);
			}
			changed = true;
		}

		changed |= DrawLayoutParamsProperty(layoutParamsObject, "Offset");

		return changed;
	}

	/// @brief The size fields every LayoutParams shares: the mode (Fixed/Content), Size, then its bounds. Under AnchoredLayoutParams,
	/// a field unused with the current anchors is greyed out (the X Size Mode on a stretched width...), and Size is a delta
	/// (dX/dY) on a stretched axis. With several nodes, a field is greyed out when none of them uses it.
	/// @param layoutParamsList the nodes' ones, all of the same type
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawSizeFields(const Vector<ui2::LayoutParams*>& layoutParamsList, EditorReflectedObject& layoutParamsObject)
	{
		bool changed = false;

		bool anyStretchedX = false;
		bool anyStretchedY = false;
		bool allStretchedX = true;
		bool allStretchedY = true;
		bool anyFixedX = false;
		bool anyFixedY = false;
		for (const ui2::LayoutParams* layoutParams : layoutParamsList)
		{
			const ui2::AnchoredLayoutParams* anchoredLayoutParams = ui2::LayoutParams::Cast<ui2::AnchoredLayoutParams>(layoutParams);
			bool                             stretchedX = anchoredLayoutParams != nullptr && anchoredLayoutParams->IsStretchedX();
			bool                             stretchedY = anchoredLayoutParams != nullptr && anchoredLayoutParams->IsStretchedY();
			anyStretchedX |= stretchedX;
			anyStretchedY |= stretchedY;
			allStretchedX &= stretchedX;
			allStretchedY &= stretchedY;
			anyFixedX |= (stretchedX == false && layoutParams->GetWidthMode() == ui2::LayoutParams::SizeMode::Fixed);
			anyFixedY |= (stretchedY == false && layoutParams->GetHeightMode() == ui2::LayoutParams::SizeMode::Fixed);
		}

		changed |= DrawSizeModeRow(layoutParamsObject, allStretchedX == false, allStretchedY == false, "Unused: the width is stretched, the anchors and the size delta give it",
		                           "Unused: the height is stretched, the anchors and the size delta give it");

		// Size, by hand rather than reflected to tell its components apart: a delta added to the anchors' span on a stretched axis (dX/dY), the node's own size on the others
		if (EditorReflectedProperty* sizeProperty = layoutParamsObject.FindProperty("Size"))
		{
			// A Content axis anchored on a point takes its size from the content: that component is unused
			bool usedX = anyFixedX || anyStretchedX;
			bool usedY = anyFixedY || anyStretchedY;
			bool used = usedX || usedY;
			ImGui::PushID(sizeProperty->GetReflectionProperty());
			ImGui::BeginDisabled(used == false);
			changed |= PropertyDrawer::BeginProperty(*sizeProperty);
			if (used == false)
			{
				ImGui::SetItemTooltip("Unused: no axis is Fixed or stretched (sized from the content)");
			}
			else if (anyStretchedX || anyStretchedY)
			{
				ImGui::SetItemTooltip("dX/dY: added to the anchors' span on a stretched axis, around the alignment point");
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

			// Each component on its own: the nodes may differ on one only
			Vector2 sizeValue = *sizeProperty->GetObject<Vector2>();
			uint8_t mixedComponents = 0;
			for (uint32_t index = 1; index < sizeProperty->GetInstances().Size(); ++index)
			{
				const Vector2& otherSize = *sizeProperty->GetObjectOf<Vector2>(index);
				mixedComponents |= (otherSize.GetX() != sizeValue.GetX() ? 1 : 0) | (otherSize.GetY() != sizeValue.GetY() ? 2 : 0);
			}
			uint8_t editedComponents = 0;
			if (DrawHelper::DrawVector2(sizeValue, allStretchedX ? "dX" : "X", allStretchedY ? "dY" : "Y", usedX, usedY, mixedComponents, &editedComponents))
			{
				sizeProperty->EditObjects<Vector2>(
					[&](Vector2& size)
					{
						if (editedComponents & 1)
						{
							size.SetX(sizeValue.GetX());
						}
						if (editedComponents & 2)
						{
							size.SetY(sizeValue.GetY());
						}
					});
				changed = true;
			}
			ImGui::EndDisabled();
			ImGui::PopID();
		}

		changed |= DrawLayoutParamsProperty(layoutParamsObject, "MinSize");
		changed |= DrawLayoutParamsProperty(layoutParamsObject, "MaxSize");

		return changed;
	}

	/// @brief What the nodes' type adds (Text, Image...): the type they share, with several nodes. Titled after that type; converting
	/// a node to another type is an action of the header (see UIPrefabEditorTab::ConvertNode), not a field, for a single node. Only
	/// the header for a plain Node, which adds nothing.
	/// @param convertDescriptor set to the type picked from Convert To, if any
	/// @return true if something was edited
	bool UIPrefabEditorInspectorWindow::DrawContentBlock(const Vector<ui2::Node*>& nodes, EditorReflectedObject& nodeObject, const ReflectionDescriptor*& convertDescriptor)
	{
		bool changed = false;

		const ReflectionDescriptor& currentDescriptor = *nodeObject.GetReflectionDescriptor();
		bool                        convertRequested = false;
		// An instance's nodes have their prefab's types
		bool canConvert = (nodes.Size() == 1 && UIPrefabEditorTab::CanEditStructure(nodes[0]));

		// Nodes of different types: the block only has what their types share
		String title = currentDescriptor.GetDisplayName();
		for (const ui2::Node* node : nodes)
		{
			if (node->GetReflectionDescriptorV().GetType() != currentDescriptor.GetType())
			{
				title += " (shared by their types)";
				break;
			}
		}

		bool blockOpened = DrawHelper::BeginInspectorBlock((const void*)BlockNode, DrawHelper::GetTypeIcon(currentDescriptor, ICON_MDI_VECTOR_SQUARE), title.CStr(), nullptr,
		                                                   nullptr, canConvert ? ICON_MDI_SWAP_HORIZONTAL : nullptr, canConvert ? "Convert To" : nullptr,
		                                                   canConvert ? &convertRequested : nullptr);
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
			// Greyed out when all the nodes have an Interaction
			bool allInteractions = true;
			for (const ui2::Node* node : nodes)
			{
				allInteractions &= (node->GetInteraction() != nullptr);
			}

			for (EditorReflectedProperty* property : nodeObject.GetProperties())
			{
				std::string_view name = property->GetReflectionProperty()->GetName();
				if (name == PointerTargetProperty)
				{
					ImGui::BeginDisabled(allInteractions);
					changed |= PropertyDrawer::DrawProperty(*property);
					ImGui::EndDisabled();
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
					{
						ImGui::SetTooltip(allInteractions ? "Always on with an Interaction"
						                                  : "Whether the pointer stops on this node: what is under it doesn't get it (always on with an Interaction)");
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

	/// @brief Interaction attached to the nodes (how they react to the pointer). Shown as a block when they all have one of the same type,
	/// removable from the header's close button, otherwise, when none has one, an "Add Interaction" button is shown. A Button's state
	/// overrides are edited one node at a time.
	/// @return true if the nodes' interactions were added, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawInteractionBlock(const Vector<ui2::Node*>& nodes, ui2::Node* sourceNode)
	{
		bool                  changed = false;
		ReflectionDescriptor* newInteractionDescriptor = nullptr;
		bool                  removeInteraction = false;

		// An instance's nodes have their prefab's Interactions: only their properties can be overridden, not the states
		bool                      canEditStructure = true;
		bool                      noInteraction = true;
		Vector<ui2::Interaction*> interactions;
		for (ui2::Node* node : nodes)
		{
			canEditStructure &= UIPrefabEditorTab::CanEditStructure(node);
			noInteraction &= (node->GetInteraction() == nullptr);
			interactions.PushBack(node->GetInteraction());
		}

		if (ReflectionDescriptor* interactionDescriptor = FindSharedDescriptor(interactions))
		{
			String title = String("Interaction: ") + interactionDescriptor->GetDisplayName();
			if (DrawHelper::BeginInspectorBlock((const void*)BlockInteraction, DrawHelper::GetTypeIcon(*interactionDescriptor, ICON_MDI_GESTURE_TAP), title.CStr(), nullptr,
			                                    canEditStructure ? &removeInteraction : nullptr))
			{
				ui2::Interaction* sourceInteraction = (sourceNode != nullptr) ? sourceNode->GetInteraction() : nullptr;
				if (sourceInteraction != nullptr && sourceInteraction->GetReflectionDescriptorV().GetType() != interactionDescriptor->GetType())
				{
					sourceInteraction = nullptr;
				}
				EditorReflectedObject interactionObject(ToInstances(interactions), interactionDescriptor, sourceInteraction, this);
				{
					// Its node references (a Slider's fill, a ScrollView's content) pick among the nodes of its prefab
					NodeRefCustomPropertyDrawer::Scope scope(*GetOwner<UIPrefabEditorTab>(), *nodes[0]->GetScopeRoot());
					changed |= PropertyDrawer::DrawDescriptor(interactionObject);
				}

				if (ui2::Selectable* selectable = ui2::Interaction::Cast<ui2::Selectable>(interactions[0]))
				{
					if (nodes.Size() > 1)
					{
						ImGui::TextDisabled("Looks: edited one node at a time");
					}
					else
					{
						ImGui::BeginDisabled(canEditStructure == false);
						changed |= DrawLooks(nodes[0], *selectable);
						ImGui::EndDisabled();
					}
				}
			}
			DrawHelper::EndInspectorBlock();
		}
		else if (noInteraction == false)
		{
			DrawDifferentTypes("Their Interactions");
		}
		else if (canEditStructure)
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

		// Applied once everything referencing the current interactions has been drawn: SetInteraction destroys them.
		if (removeInteraction)
		{
			for (ui2::Node* node : nodes)
			{
				node->SetInteraction(nullptr);
			}
			changed = true;
		}
		else if (newInteractionDescriptor != nullptr)
		{
			for (ui2::Node* node : nodes)
			{
				node->SetInteraction(newInteractionDescriptor->CreateInstance<ui2::Interaction>());
			}
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
	}

	/// @brief A Selectable's looks (see ui2::Selectable::GetLookCount): its states, then its type's own (a Toggle's Off and On).
	/// Each but the ones as authored (Normal, Off) has its list of property overrides, each a target node, one of its properties
	/// and the value it takes. Seen in play mode. Each can be recorded (see UIPrefabEditorTab::StartRecording): its overrides then
	/// follow the edits of the nodes, and are only removable here (which puts the property back). The other looks can't be edited
	/// meanwhile. Every look also picks the clip of the prefab it plays when entered.
	/// @return true if an override was added, removed or edited, or a clip picked
	bool UIPrefabEditorInspectorWindow::DrawLooks(ui2::Node* node, ui2::Selectable& selectable)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		bool               changed = false;

		for (uint32_t look = 0; look < selectable.GetLookCount(); ++look)
		{
			const char* lookName = selectable.GetLookName(look);
			ImGui::PushID((int)look);

			bool recording = tab->IsRecording(node, look);
			bool otherRecorded = tab->IsRecording() && recording == false;

			ImGui::BeginDisabled(otherRecorded);

			// One framed block per look (red framed while recorded): its name followed by its record button, then its overrides
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
				ImGui::TextColored(ImVec4(1.0f, 80.0f / 255.0f, 80.0f / 255.0f, 1.0f), "%s", lookName);
			}
			else
			{
				ImGui::TextUnformatted(lookName);
			}
			// Normal (or Off) has no override: it is the prefab as authored
			bool hasOverrides = selectable.GetLookOverrides(look) != nullptr;
			if (hasOverrides)
			{
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
						tab->StartRecording(node, look);
					}
				}
				if (recording)
				{
					ImGui::PopStyleColor();
				}
				ImGui::SetItemTooltip(recording ? "Stop recording, back to the prefab as authored"
				                                : "Record: show this look, and record the properties edited on any node (inspector) as its overrides");
			}
			ImGui::Separator();

			// The clip played when entering the look: one of the prefab's (held by the selectable's node or an ancestor, up to the
			// root of its prefab, see ui2::Node::FindAnimationOwner)
			if (String* lookAnimation = selectable.GetLookAnimation(look))
			{
				const String     animation = *lookAnimation; // a copy: the combo sets it
				std::string_view animationName(animation.CStr(), animation.Length());
				bool             missing = animation.Empty() == false && node->FindAnimationOwner(animationName) == nullptr;
				String           preview = animation.Empty() ? String("No animation") : (missing ? animation + " (missing)" : animation);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(ICON_MDI_ANIMATION_PLAY);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (missing)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 80, 80, 255));
				}
				bool open = ImGui::BeginCombo("##Animation", preview.CStr());
				if (missing)
				{
					ImGui::PopStyleColor();
				}
				if (open)
				{
					if (ImGui::Selectable("No animation", animation.Empty()) && animation.Empty() == false)
					{
						*lookAnimation = String();
						changed = true;
					}
					for (ui2::Node* owner = node; owner != nullptr; owner = owner->GetParent())
					{
						for (const ui2::AnimationClip& clip : owner->GetAnimations())
						{
							if (ImGui::Selectable(clip._name.CStr(), clip._name == animation) && clip._name != animation)
							{
								*lookAnimation = clip._name;
								changed = true;
							}
						}
						if (owner->GetPrefabInstance() != nullptr)
						{
							break; // the root of its prefab
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SetItemTooltip("The animation played when entering the look (the previous one's is stopped). Made in the Animation window");
			}

			if (hasOverrides == false)
			{
				ImGui::EndChild();
				ImGui::EndDisabled();
				ImGui::PopID();
				continue;
			}
			ImGui::Separator();

			Vector<ui2::PropertyOverride>& overrides = *selectable.GetLookOverrides(look);
			int32_t                        removeIndex = -1;
			for (uint32_t index = 0; index < overrides.Size(); ++index)
			{
				ImGui::PushID((int)index);
				bool removeRequested = false;
				DrawPropertyOverride(node, overrides[index], recording, removeRequested);
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
	/// @param owner the Button's node, whose prefab the target is resolved in (an instance's own, for an instance's Button)
	/// @param propertyOverride
	/// @param recording whether its state is being recorded (its remove button then puts the property back)
	/// @param removeRequested set when its remove button is clicked
	void UIPrefabEditorInspectorWindow::DrawPropertyOverride(ui2::Node* owner, const ui2::PropertyOverride& propertyOverride, bool recording, bool& removeRequested)
	{
		ui2::Node* target = owner->GetScopeRoot()->FindByLocalId(propertyOverride._target);

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
		String valueText = FormatSerializedValue(propertyOverride._value.GetRootNode());
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
			if (const ui2::Selectable* selectable = node->GetInteraction<ui2::Selectable>())
			{
				text += String(" (") + selectable->GetLookName((uint32_t)selectable->GetState()) + ")";
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

	/// @brief View attached to `node` (the prefab's code, see ui2::View): its properties, the node references first among them.
	/// Added to the prefab's root only (a view found on another node is still shown), from the views the game modules register.
	/// A view of an unknown type (its module not loaded) is kept as is, only removable. An instance root's view is its prefab's:
	/// read-only.
	/// @return true if the node's view was added, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawViewBlock(ui2::Node* node)
	{
		UIPrefabEditorTab*    tab = GetOwner<UIPrefabEditorTab>();
		bool                  changed = false;
		ReflectionDescriptor* newViewDescriptor = nullptr;
		bool                  removeView = false;
		bool                  canEditStructure = UIPrefabEditorTab::CanEditStructure(node);

		ui2::View* view = node->GetView();
		if (view != nullptr)
		{
			ReflectionDescriptor& currentDescriptor = view->GetReflectionDescriptorV();

			String title = String("View: ") + currentDescriptor.GetDisplayName();
			if (DrawHelper::BeginInspectorBlock((const void*)BlockView, DrawHelper::GetTypeIcon(currentDescriptor, ICON_MDI_CODE_BRACES), title.CStr(), nullptr,
			                                    canEditStructure ? &removeView : nullptr))
			{
				ImGui::BeginDisabled(canEditStructure == false);
				NodeRefCustomPropertyDrawer::Scope scope(*tab, *node->GetScopeRoot());
				EditorReflectedObject              viewObject(view, &currentDescriptor, nullptr, this);
				changed |= PropertyDrawer::DrawDescriptor(viewObject);
				ImGui::EndDisabled();
			}
			DrawHelper::EndInspectorBlock();
		}
		else if (node->HasUnknownView())
		{
			if (DrawHelper::BeginInspectorBlock((const void*)BlockView, ICON_MDI_CODE_BRACES, "View: unknown type", nullptr, canEditStructure ? &removeView : nullptr))
			{
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ICON_MDI_ALERT " Its game module isn't loaded");
				ImGui::TextDisabled("Kept as is until then");
			}
			DrawHelper::EndInspectorBlock();
		}
		else if (canEditStructure && node == tab->GetPrefabRoot())
		{
			ImVec2 addButtonSize = ImGui::CalcTextSize("  Add View  ") + ImVec2(35.0f, 15.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * 0.5f - addButtonSize.x * 0.5f));
			if (ImGui::Button("  Add View  ", addButtonSize))
			{
				ImGui::OpenPopup("UIPrefabAddView");
			}
			ImGui::SetItemTooltip("The code gameplay drives the prefab through (a ui2::View of the game module)");
			if (ImGui::BeginPopup("UIPrefabAddView"))
			{
				const auto& viewDescriptors = ui2::ViewFactory::GetInstance()->GetAllDescriptors();
				if (viewDescriptors.empty())
				{
					ImGui::TextDisabled("No view: a game module registers its own (ui2::ViewFactory)");
				}
				for (const auto& descriptorPair : viewDescriptors)
				{
					String label = String(DrawHelper::GetTypeIcon(*descriptorPair.second, ICON_MDI_CODE_BRACES)) + " " + descriptorPair.second->GetDisplayName();
					if (ImGui::MenuItem(label.CStr()))
					{
						newViewDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndPopup();
			}
			ImGui::Spacing();
		}

		// Applied once everything referencing the current view has been drawn: SetView destroys it.
		if (removeView)
		{
			node->SetView(nullptr);
			changed = true;
		}
		else if (newViewDescriptor != nullptr)
		{
			node->SetView(newViewDescriptor->CreateInstance<ui2::View>());
			changed = true;
		}

		return changed;
	}

	/// @brief Layout attached to the nodes (how their own children are arranged). Shown as a block when they all have one of the
	/// same type — its type can be changed from the combo (transform into another layout) and it can be removed from the header's
	/// close button — otherwise, when none has one, an "Add Children Layout" button is shown.
	/// Node::SetLayout takes care of converting the children's LayoutParams to the new layout's type.
	/// @return true if the nodes' layouts were added, replaced, removed or edited
	bool UIPrefabEditorInspectorWindow::DrawLayoutBlock(const Vector<ui2::Node*>& nodes, ui2::Node* sourceNode)
	{
		bool                  changed = false;
		ReflectionDescriptor* newLayoutDescriptor = nullptr;
		bool                  removeLayout = false;

		// An instance's nodes have their prefab's layouts: only their properties can be overridden
		bool                 canEditStructure = true;
		bool                 noLayout = true;
		Vector<ui2::Layout*> layouts;
		for (ui2::Node* node : nodes)
		{
			canEditStructure &= UIPrefabEditorTab::CanEditStructure(node);
			noLayout &= (node->GetLayout() == nullptr);
			layouts.PushBack(node->GetLayout());
		}

		const auto& layoutDescriptors = ui2::LayoutFactory::GetInstance()->GetAllDescriptors();

		if (ReflectionDescriptor* layoutDescriptor = FindSharedDescriptor(layouts))
		{
			String layoutTitle = String("Children Layout: ") + layoutDescriptor->GetDisplayName();
			bool   convertRequested = false;
			bool   blockOpened = DrawHelper::BeginInspectorBlock((const void*)BlockLayout, ICON_MDI_VIEW_QUILT, layoutTitle.CStr(), nullptr,
			                                                     canEditStructure ? &removeLayout : nullptr, canEditStructure ? ICON_MDI_SWAP_HORIZONTAL : nullptr,
			                                                     canEditStructure ? "Convert To" : nullptr, canEditStructure ? &convertRequested : nullptr);
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
					bool isCurrent = (descriptorPair.second->GetType() == layoutDescriptor->GetType());
					if (ImGui::MenuItem(descriptorPair.second->GetDisplayName().CStr(), nullptr, isCurrent, isCurrent == false))
					{
						newLayoutDescriptor = descriptorPair.second;
					}
				}
				ImGui::EndPopup();
			}
			if (blockOpened)
			{
				ui2::Layout* sourceLayout = (sourceNode != nullptr) ? sourceNode->GetLayout() : nullptr;
				if (sourceLayout != nullptr && sourceLayout->GetReflectionDescriptorV().GetType() != layoutDescriptor->GetType())
				{
					sourceLayout = nullptr;
				}
				EditorReflectedObject layoutObject(ToInstances(layouts), layoutDescriptor, sourceLayout, this);
				changed |= PropertyDrawer::DrawDescriptor(layoutObject);
			}
			DrawHelper::EndInspectorBlock();
		}
		else if (noLayout == false)
		{
			DrawDifferentTypes("Their Children Layouts");
		}
		else if (canEditStructure)
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

		// Applied once everything referencing the current layouts has been drawn: SetLayout destroys them.
		if (removeLayout)
		{
			for (ui2::Node* node : nodes)
			{
				node->SetLayout(nullptr);
			}
			changed = true;
		}
		else if (newLayoutDescriptor != nullptr)
		{
			for (ui2::Node* node : nodes)
			{
				node->ConvertLayout(*newLayoutDescriptor);
			}
			changed = true;
		}

		return changed;
	}
}
