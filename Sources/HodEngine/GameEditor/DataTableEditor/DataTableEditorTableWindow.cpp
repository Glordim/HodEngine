#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTableWindow.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(DataTableEditorTableWindow) {}

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
		if (editorTab->IsReadable() == false)
		{
			ImGui::TextUnformatted("Its row type is unknown: its module isn't loaded");
			return;
		}

		DataTableAssetData& data = editorTab->GetData();

		bool changed = false;

		if (ImGui::Button(ICON_MDI_PLUS " Add Row") == true)
		{
			uint32_t suffix = 0;
			String   newKey = "NewRow";
			while (editorTab->FindRow(newKey) != nullptr)
			{
				++suffix;
				newKey = String::Format("NewRow%u", suffix);
			}
			editorTab->AddRow(newKey);
			editorTab->SetRowSelection(newKey);
			changed = true;
		}

		ReflectionDescriptor* reflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(data._rowType);
		if (reflectionDescriptor != nullptr)
		{
			uint32_t columnCount = 1 + reflectionDescriptor->GetProperties().Size();
			if (ImGui::BeginTable("DataTableRows", columnCount, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_PadOuterX) == true)
			{
				ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
				for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
				{
					ImGui::TableSetupColumn(reflectionProperty->GetDisplayName().CStr(), ImGuiTableColumnFlags_WidthStretch);
				}
				ImGui::TableHeadersRow();

				const String& selectedKey = editorTab->GetRowSelectionKey();
				const String* keyToDelete = nullptr;

				for (DataTableAssetData::Row& row : data._rows)
				{
					ImGui::PushID(row._key.CStr());
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					row._key.Reserve(255);
					if (ImGui::InputText("##key", row._key.Data(), row._key.Capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						row._key = row._key.Data(); // refresh size
						changed |= true;
					}

					EditorReflectedObject reflectedObject(row._data, reflectionDescriptor, nullptr, this);
					for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
					{
						ImGui::TableNextColumn();
						EditorReflectedProperty* reflectedProperty = reflectedObject.FindProperty(reflectionProperty->GetName());
						if (reflectedProperty != nullptr)
						{
							changed |= PropertyDrawer::DrawProperty(*reflectedProperty, true);
						}
					}

					ImGui::TableSetColumnIndex(0);
					ImGuiTable* table = ImGui::GetCurrentTable();
					float rowHeight = table->RowPosY2 - table->RowPosY1;
					bool selected = (row._key == selectedKey);
					if (ImGui::Selectable("##row_select", selected, 
										ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, 
										ImVec2(0, rowHeight - 20)))
					{
						editorTab->SetRowSelection(row._key);
					}

					ImGui::PopID();
				}

				if (keyToDelete != nullptr)
				{
					String deletedKey = *keyToDelete;
					editorTab->RemoveRow(deletedKey);
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
