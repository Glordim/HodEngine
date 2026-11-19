#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTableWindow.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <cfloat>
#include <cstdint>
#include <cstdio>

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
			editorTab->MarkAsDirty("Add Row");
		}

		// The selected row (also its context menu in the table)
		ImGui::SameLine();
		String selectedKey = editorTab->GetRowSelectionKey();
		ImGui::BeginDisabled(editorTab->FindRow(selectedKey) == nullptr);
		if (ImGui::Button(ICON_MDI_DELETE " Remove Row") == true)
		{
			editorTab->RemoveRow(selectedKey);
			editorTab->MarkAsDirty("Remove Row");
		}
		ImGui::EndDisabled();

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

				String keyToRemove; // After the loop: the rows aren't changed while drawn

				for (uint32_t rowIndex = 0; rowIndex < data._rows.Size(); ++rowIndex)
				{
					DataTableAssetData::Row& row = data._rows[rowIndex];

					ImGui::PushID(static_cast<int>(rowIndex)); // Not the key: it changes while edited
					ImGui::TableNextRow();

					// The key, applied once its edit is done (Enter or focus lost), refused when empty or taken (kept as it was)
					ImGui::TableNextColumn();
					char keyBuffer[256];
					std::snprintf(keyBuffer, sizeof(keyBuffer), "%s", row._key.CStr());
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputText("##key", keyBuffer, sizeof(keyBuffer));
					if (ImGui::IsItemDeactivatedAfterEdit() && row._key != keyBuffer && editorTab->RenameRow(row._key, keyBuffer))
					{
						editorTab->MarkAsDirty("Rename Row");
					}

					EditorReflectedObject reflectedObject(row._data, reflectionDescriptor, nullptr, this);
					for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
					{
						ImGui::TableNextColumn();
						EditorReflectedProperty* reflectedProperty = reflectedObject.FindProperty(reflectionProperty->GetName());
						if (reflectedProperty != nullptr)
						{
							changed |= ReflectionDrawer::DrawProperty(*reflectedProperty, true);
						}
					}

					ImGui::TableSetColumnIndex(0);
					ImGuiTable* table = ImGui::GetCurrentTable();
					float rowHeight = table->RowPosY2 - table->RowPosY1;
					bool selected = (row._key == editorTab->GetRowSelectionKey());
					if (ImGui::Selectable("##row_select", selected,
										ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
										ImVec2(0, rowHeight - 20)))
					{
						editorTab->SetRowSelection(row._key);
					}
					if (ImGui::BeginPopupContextItem("##row_menu"))
					{
						if (ImGui::MenuItem(ICON_MDI_DELETE " Remove Row"))
						{
							keyToRemove = row._key;
						}
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}

				if (keyToRemove.Empty() == false)
				{
					editorTab->RemoveRow(keyToRemove);
					editorTab->MarkAsDirty("Remove Row");
				}

				ImGui::EndTable();
			}
		}

		if (changed)
		{
			editorTab->MarkAsDirty("Edit Row");
		}
	}
}
