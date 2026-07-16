#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTableWindow.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Game/DataStructFactory.hpp"
#include "HodEngine/Game/DataTableContainer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(DataTableEditorTableWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	DataTableEditorTableWindow::DataTableEditorTableWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Table");
	}

	/// @brief
	void DataTableEditorTableWindow::DrawContent()
	{
		DataTableEditorTab* editorTab = GetOwner<DataTableEditorTab>();
		DataTableContainer& dataTableContainer = editorTab->GetDataTableContainer();

		bool changed = false;

		if (ImGui::Button(ICON_MDI_PLUS " Add Row") == true)
		{
			uint32_t suffix = 0;
			String   newKey = "NewRow";
			while (dataTableContainer.FindRow(newKey) != nullptr)
			{
				++suffix;
				newKey = String::Format("NewRow%u", suffix);
			}
			dataTableContainer.AddRow(newKey);
			editorTab->SetRowSelection(newKey);
			changed = true;
		}

		std::vector<const String*> keys;
		keys.reserve(dataTableContainer.GetRows().size());
		for (const auto& pair : dataTableContainer.GetRows())
		{
			keys.push_back(&pair.first);
		}
		std::sort(keys.begin(), keys.end(), [](const String* left, const String* right) { return *left < *right; });

		ReflectionDescriptor* reflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(dataTableContainer.GetRowType());
		if (reflectionDescriptor != nullptr)
		{
			uint32_t columnCount = 1 + reflectionDescriptor->GetProperties().Size();
			if (ImGui::BeginTable("DataTableRows", columnCount, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders) == true)
			{
				ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
				for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
				{
					ImGui::TableSetupColumn(reflectionProperty->GetDisplayName().CStr(), ImGuiTableColumnFlags_WidthStretch);
				}
				ImGui::TableHeadersRow();

				const String& selectedKey = editorTab->GetRowSelectionKey();
				const String* keyToDelete = nullptr;

				DataTableContainer& dataTable = editorTab->GetDataTableContainer();

				for (const auto& pair : dataTable.GetRows())
				{
					ImGui::PushID(pair.first.CStr());
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					bool selected = (pair.first == selectedKey);
					if (ImGui::Selectable(pair.first.CStr(), selected, ImGuiSelectableFlags_SpanAllColumns) == true)
					{
						editorTab->SetRowSelection(pair.first);
					}

					EditorReflectedObject reflectedObject(pair.second, reflectionDescriptor, nullptr, this);
					for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
					{
						ImGui::TableNextColumn();
						EditorReflectedProperty* reflectedProperty = reflectedObject.FindProperty(reflectionProperty->GetName());
						if (reflectedProperty != nullptr)
						{
							changed |= PropertyDrawer::DrawProperty(*reflectedProperty);
						}
					}

					ImGui::PopID();
				}

				if (keyToDelete != nullptr)
				{
					String deletedKey = *keyToDelete;
					dataTableContainer.RemoveRow(deletedKey);
					if (editorTab->GetRowSelectionKey() == deletedKey)
					{
						editorTab->SetRowSelection("");
					}
					editorTab->MarkAsDirty();
				}

				ImGui::EndTable();
			}
		}

		if (changed)
		{
			editorTab->MarkAsDirty();
		}
	}
}
