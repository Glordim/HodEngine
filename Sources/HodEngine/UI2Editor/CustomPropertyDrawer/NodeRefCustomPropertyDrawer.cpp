#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeRef.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>

#include <algorithm>

namespace hod::inline editor
{
	namespace
	{
		UIPrefabEditorTab* scopeTab = nullptr;
		ui2::Node*         scopeRoot = nullptr;

		/// @brief The nodes a reference of the scope can point at (see ui2::Node::FindByLocalId): the scope root's descendants,
		/// a nested instance's root but not its content. Not the canvas' hidden container.
		void CollectScopeNodes(ui2::Node& node, bool isScopeRoot, Vector<ui2::Node*>& outNodes)
		{
			if (node.GetParent() != nullptr)
			{
				outNodes.PushBack(&node);
			}
			if (isScopeRoot == false && node.GetPrefabInstance() != nullptr)
			{
				return;
			}
			for (ui2::Node* child : node.GetChildren())
			{
				CollectScopeNodes(*child, false, outNodes);
			}
		}

		/// @brief Its icon and name
		String GetNodeLabel(const ui2::Node& node)
		{
			String name = node.GetName().Empty() ? String("(unnamed)") : node.GetName();
			return String(DrawHelper::GetTypeIcon(node.GetReflectionDescriptorV(), ICON_MDI_VECTOR_SQUARE)) + " " + name;
		}

		/// @brief The node's id seen from the scope: an instance root scoping its own references is seen by its id in its prefab
		uint64_t GetScopeLocalId(const ui2::Node& node)
		{
			if (&node == scopeRoot && node.GetPrefabInstance() != nullptr)
			{
				return node.GetPrefabInstance()->GetRootLocalId();
			}
			return node.GetLocalId();
		}

		/// @brief References node, giving the nodes their local ids first if they don't have one yet
		void Assign(ui2::NodeRefBase& reference, ui2::Node& node)
		{
			if (node.GetLocalId() == 0)
			{
				scopeTab->AssignLocalIds();
			}
			reference.SetLocalId(GetScopeLocalId(node));
		}
	}

	/// @brief
	/// @param tab
	/// @param root
	NodeRefCustomPropertyDrawer::Scope::Scope(UIPrefabEditorTab& tab, ui2::Node& root)
	{
		scopeTab = &tab;
		scopeRoot = &root;
	}

	/// @brief
	NodeRefCustomPropertyDrawer::Scope::~Scope()
	{
		scopeTab = nullptr;
		scopeRoot = nullptr;
	}

	/// @brief
	/// @param editorReflectedProperty
	/// @param onlyValue
	/// @return
	bool NodeRefCustomPropertyDrawer::Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		bool changed = false;
		if (onlyValue == false)
		{
			changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
		}

		ui2::NodeRefBase* reference = editorReflectedProperty.GetObject<ui2::NodeRefBase>();
		ImGui::PushID(reference);

		const ReflectionDescriptor* expectedDescriptor = (reference->GetViewDescriptor() != nullptr) ? reference->GetViewDescriptor() : reference->GetNodeDescriptor();
		String                      expectedName = (expectedDescriptor != nullptr) ? expectedDescriptor->GetDisplayName() : String("Node");

		if (scopeRoot == nullptr)
		{
			ImGui::TextDisabled("%llu", (unsigned long long)reference->GetLocalId());
			ImGui::PopID();
			return changed;
		}

		ui2::Node* referenced = scopeRoot->FindByLocalId(reference->GetLocalId());
		bool       missing = (reference->GetLocalId() != 0 && (referenced == nullptr || reference->Accepts(*referenced) == false));

		String preview = "None";
		if (missing)
		{
			preview = (referenced != nullptr) ? String(ICON_MDI_ALERT " ") + referenced->GetName() + " (not a " + expectedName + ")" : String(ICON_MDI_ALERT " Missing node");
		}
		else if (referenced != nullptr)
		{
			preview = GetNodeLabel(*referenced);
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_CURSOR_POINTER).x - ImGui::GetStyle().ItemSpacing.x);
		if (missing)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		}
		bool opened = ImGui::BeginCombo("##Node", preview.CStr(), ImGuiComboFlags_HeightLarge);
		if (missing)
		{
			ImGui::PopStyleColor();
		}
		ImGui::SetItemTooltip("A %s of the prefab: pick it, or drop it from the hierarchy", expectedName.CStr());
		if (opened)
		{
			if (ImGui::Selectable("None", reference->GetLocalId() == 0))
			{
				reference->SetLocalId(0);
				changed = true;
			}

			Vector<ui2::Node*> nodes;
			CollectScopeNodes(*scopeRoot, true, nodes);
			for (ui2::Node* node : nodes)
			{
				if (reference->Accepts(*node) == false)
				{
					continue;
				}
				ImGui::PushID(node);
				if (ImGui::Selectable(GetNodeLabel(*node).CStr(), node == referenced))
				{
					Assign(*reference, *node);
					changed = true;
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		// A node dragged from the hierarchy
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* peekedPayload = ImGui::GetDragDropPayload();
			if (peekedPayload != nullptr && peekedPayload->IsDataType("UI2Node"))
			{
				ui2::Node*         node = *static_cast<ui2::Node* const*>(peekedPayload->Data);
				Vector<ui2::Node*> nodes;
				CollectScopeNodes(*scopeRoot, true, nodes);
				bool inScope = std::find(nodes.Begin(), nodes.End(), node) != nodes.End();
				if (inScope && reference->Accepts(*node) && ImGui::AcceptDragDropPayload("UI2Node") != nullptr)
				{
					Assign(*reference, *node);
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::BeginDisabled(referenced == nullptr);
		if (ImGui::Button(ICON_MDI_CURSOR_POINTER))
		{
			scopeTab->SetSelectedNode(referenced);
		}
		ImGui::EndDisabled();
		ImGui::SetItemTooltip("Select it");

		ImGui::PopID();
		return changed;
	}
}
