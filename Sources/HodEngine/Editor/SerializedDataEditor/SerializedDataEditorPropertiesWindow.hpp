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
	class HOD_EDITOR_API SerializedDataEditorPropertiesWindow : public EditorTabWindow
	{
		META_TYPE(SerializedDataEditorPropertiesWindow, EditorTabWindow);
		WINDOW_DESCRIPTION()

	public:

					SerializedDataEditorPropertiesWindow() = default;
					SerializedDataEditorPropertiesWindow(EditorTab* editorTab);
					~SerializedDataEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
