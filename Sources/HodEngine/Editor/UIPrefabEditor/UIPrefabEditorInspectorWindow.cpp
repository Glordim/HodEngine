#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>

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

		ImGui::SeparatorText("Node");
		EditorReflectedObject nodeObject(node, &node->GetReflectionDescriptorV(), nullptr, this);
		changed |= PropertyDrawer::DrawDescriptor(nodeObject);

		ui2::LayoutParams* layoutParams = node->GetLayoutParams();
		if (layoutParams != nullptr)
		{
			ImGui::SeparatorText("Layout");
			EditorReflectedObject layoutParamsObject(layoutParams, &layoutParams->GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(layoutParamsObject);
		}

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}
}
