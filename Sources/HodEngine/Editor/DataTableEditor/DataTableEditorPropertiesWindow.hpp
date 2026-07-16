#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include <memory>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief Draws the properties of the DataTableEditorTab's currently selected row.
	class HOD_EDITOR_API DataTableEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(DataTableEditorPropertiesWindow, EditorTabWindow)

	public:

					DataTableEditorPropertiesWindow() = default;
					DataTableEditorPropertiesWindow(EditorTab* editorTab);
					~DataTableEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
