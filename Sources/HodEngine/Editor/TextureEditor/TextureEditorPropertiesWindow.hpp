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
	class HOD_EDITOR_API TextureEditorPropertiesWindow : public EditorTabWindow
	{
		META_TYPE(TextureEditorPropertiesWindow, EditorTabWindow)
		WINDOW_DESCRIPTION()

	public:

					TextureEditorPropertiesWindow() = default;
					TextureEditorPropertiesWindow(EditorTab* editorTab);
					~TextureEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
