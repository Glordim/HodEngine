#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API MaterialEditorPropertiesWindow : public EditorTabWindow
	{
		META_TYPE(MaterialEditorPropertiesWindow, EditorTabWindow);
		WINDOW_DESCRIPTION()

	public:

					MaterialEditorPropertiesWindow() = default;
					MaterialEditorPropertiesWindow(EditorTab* editorTab);
					~MaterialEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
