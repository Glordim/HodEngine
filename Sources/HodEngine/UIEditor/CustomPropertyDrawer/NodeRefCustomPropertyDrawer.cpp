#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/CustomPropertyDrawer/NodeRefCustomPropertyDrawer.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/Widgets.hpp>
#include <HodEngine/UI/Node.hpp>
#include <HodEngine/UI/NodeRef.hpp>
#include <HodEngine/UI/PrefabInstance.hpp>

#include <algorithm>

namespace hod::inline editor
{
	namespace
	{
		UIPrefabEditorTab* scopeTab = nullptr;
		ui::Node*         scopeRoot = nullptr;

		/// @brief The nodes a reference of the scope can point at (see ui::Node::FindByLocalId): the scope root's descendants,
		/// a nested instance's root but not its content. Not the canvas' hidden container.
		void CollectScopeNodes(ui::Node& node, bool isScopeRoot, Vector<ui::Node*>& outNodes)
		{
			if (node.GetParent() != nullptr)
			{
				outNodes.PushBack(&node);
			}
			if (isScopeRoot == false && node.GetPrefabInstance() != nullptr)
			{
				return;
			}
			for (ui::Node* child : node.GetChildren())
			{
				CollectScopeNodes(*child, false, outNodes);
			}
		}

		/// @brief Its icon and name
		String GetNodeLabel(const ui::Node& node)
		{
			String name = node.GetName().Empty() ? String("(unnamed)") : node.GetName();
			return String(DrawHelper::GetTypeIcon(node.GetReflectionDescriptorV(), ICON_MDI_VECTOR_SQUARE)) + " " + name;
		}

		/// @brief The node's id seen from the scope: an instance root scoping its own references is seen by its id in its prefab
		uint64_t GetScopeLocalId(const ui::Node& node)
		{
			if (&node == scopeRoot && node.GetPrefabInstance() != nullptr)
			{
				return node.GetPrefabInstance()->GetRootLocalId();
			}
			return node.GetLocalId();
		}

		/// @brief References node, giving the nodes their local ids first if they don't have one yet
		void Assign(ui::NodeRefBase& reference, ui::Node& node)
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
	NodeRefCustomPropertyDrawer::Scope::Scope(UIPrefabEditorTab& tab, ui::Node& root)
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

		ui::NodeRefBase* reference = editorReflectedProperty.GetObject<ui::NodeRefBase>();
		ImGui::PushID(reference);

		const ReflectionDescriptor* expectedDescriptor = (reference->GetViewDescriptor() != nullptr) ? reference->GetViewDescriptor() : reference->GetNodeDescriptor();
		String                      expectedName = (expectedDescriptor != nullptr) ? expectedDescriptor->GetDisplayName() : String("Node");

		if (scopeRoot == nullptr)
		{
			ImGui::TextDisabled("%llu", (unsigned long long)reference->GetLocalId());
			ImGui::PopID();
			return changed;
		}

		// The references edited together, set together (in place, as they are their owners' members)
		bool mixed = editorReflectedProperty.HasDifferentValues();
		auto assignAll = [&](ui::Node* node)
		{
			for (uint32_t index = 0; index < editorReflectedProperty.GetInstances().Size(); ++index)
			{
				ui::NodeRefBase* instanceReference = editorReflectedProperty.GetObjectOf<ui::NodeRefBase>(index);
				if (node != nullptr)
				{
					Assign(*instanceReference, *node);
				}
				else
				{
					instanceReference->SetLocalId(0);
				}
			}
		};

		ui::Node* referenced = mixed ? nullptr : scopeRoot->FindByLocalId(reference->GetLocalId());
		bool       missing = (mixed == false && reference->GetLocalId() != 0 && (referenced == nullptr || reference->Accepts(*referenced) == false));

		String preview = mixed ? String(GetMixedValueLabel()) : String("None");
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
			if (ImGui::Selectable("None", mixed == false && reference->GetLocalId() == 0))
			{
				assignAll(nullptr);
				changed = true;
			}

			Vector<ui::Node*> nodes;
			CollectScopeNodes(*scopeRoot, true, nodes);
			for (ui::Node* node : nodes)
			{
				if (reference->Accepts(*node) == false)
				{
					continue;
				}
				ImGui::PushID(node);
				if (ImGui::Selectable(GetNodeLabel(*node).CStr(), node == referenced))
				{
					assignAll(node);
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
			if (peekedPayload != nullptr && peekedPayload->IsDataType("UINode"))
			{
				ui::Node*         node = *static_cast<ui::Node* const*>(peekedPayload->Data);
				Vector<ui::Node*> nodes;
				CollectScopeNodes(*scopeRoot, true, nodes);
				bool inScope = std::find(nodes.Begin(), nodes.End(), node) != nodes.End();
				if (inScope && reference->Accepts(*node) && ImGui::AcceptDragDropPayload("UINode") != nullptr)
				{
					assignAll(node);
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
