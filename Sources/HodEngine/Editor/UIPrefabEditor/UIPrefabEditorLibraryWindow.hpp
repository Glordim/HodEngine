#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

namespace hod::inline editor
{
	/// @brief Lists every ui2::Node type registered in NodeFactory (currently only the base Node
	/// itself), so a UIPrefabEditorTab user can add one as a child of the node selected in the
	/// Hierarchy window. Meant to be docked with UIPrefabEditorHierarchyWindow.
	class HOD_EDITOR_API UIPrefabEditorLibraryWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorLibraryWindow, EditorTabWindow)

	public:

					UIPrefabEditorLibraryWindow() = default;
					UIPrefabEditorLibraryWindow(EditorTab* editorTab);
					~UIPrefabEditorLibraryWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
