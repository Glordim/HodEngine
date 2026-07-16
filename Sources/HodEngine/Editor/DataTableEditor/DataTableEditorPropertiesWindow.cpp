#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/Game/DataStruct.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(DataTableEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	DataTableEditorPropertiesWindow::DataTableEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Properties");
	}

	/// @brief
	void DataTableEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;
		DataStruct* selectedRow = GetOwner<DataTableEditorTab>()->GetRowSelection();

		if (selectedRow != nullptr)
		{
			EditorReflectedObject reflectedObject(selectedRow, &selectedRow->GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}
}
