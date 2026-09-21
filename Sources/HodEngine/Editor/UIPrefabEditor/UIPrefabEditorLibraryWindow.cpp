#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorLibraryWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorLibraryWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	UIPrefabEditorLibraryWindow::UIPrefabEditorLibraryWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Library");
	}

	/// @brief
	void UIPrefabEditorLibraryWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		for (const auto& nodeDescriptorPair : NodeFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor& nodeDescriptor = *nodeDescriptorPair.second;

			ImGui::Selectable(nodeDescriptor.GetDisplayName().CStr());

			if (ImGui::BeginDragDropSource())
			{
				ReflectionDescriptor* nodeDescriptorPtr = &nodeDescriptor;
				ImGui::SetDragDropPayload("UI2NodeLibraryDescriptor", &nodeDescriptorPtr, sizeof(ReflectionDescriptor*));
				ImGui::TextUnformatted(nodeDescriptor.GetDisplayName().CStr());
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				ui2::Node* parent = tab->GetSelectedNode();
				if (parent == nullptr)
				{
					parent = tab->GetPrefabRoot();
				}

				parent->AddChild(NodeFactory::GetInstance()->CreateNode(nodeDescriptor));
				tab->MarkAsDirty();
			}
		}
	}
}
