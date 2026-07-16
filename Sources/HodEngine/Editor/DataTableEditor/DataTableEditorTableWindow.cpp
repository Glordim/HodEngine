#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTableWindow.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTab.hpp"

#include "HodEngine/Game/DataTableContainer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <algorithm>
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
		DataTableContainer&  dataTableContainer = editorTab->GetDataTableContainer();

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
			editorTab->MarkAsDirty();
		}

		std::vector<const String*> keys;
		keys.reserve(dataTableContainer.GetRows().size());
		for (const auto& pair : dataTableContainer.GetRows())
		{
			keys.push_back(&pair.first);
		}
		std::sort(keys.begin(), keys.end(), [](const String* left, const String* right) { return *left < *right; });

		if (ImGui::BeginTable("DataTableRows", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders) == true)
		{
			ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("##Delete", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 24.0f);
			ImGui::TableHeadersRow();

			const String& selectedKey = editorTab->GetRowSelectionKey();
			const String* keyToDelete = nullptr;

			for (const String* key : keys)
			{
				ImGui::PushID(key);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				bool selected = (*key == selectedKey);
				if (ImGui::Selectable(key->CStr(), selected, ImGuiSelectableFlags_SpanAllColumns) == true)
				{
					editorTab->SetRowSelection(*key);
				}

				ImGui::TableNextColumn();
				if (ImGui::SmallButton(ICON_MDI_DELETE) == true)
				{
					keyToDelete = key;
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
}
