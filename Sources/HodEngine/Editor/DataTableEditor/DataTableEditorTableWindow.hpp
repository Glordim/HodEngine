#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include <memory>

namespace hod::inline editor
{
	/// @brief Displays the rows of a DataTableContainer in an ImGui table, letting the user select
	/// the row whose properties get shown in the DataTableEditorPropertiesWindow.
	class HOD_EDITOR_API DataTableEditorTableWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(DataTableEditorTableWindow, EditorTabWindow)

	public:

					DataTableEditorTableWindow() = default;
					DataTableEditorTableWindow(EditorTab* editorTab);
					~DataTableEditorTableWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
