#pragma once
#include "HodEngine/RendererEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_RENDERER_EDITOR_API TextureEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(TextureEditorPropertiesWindow, EditorTabWindow)

	public:

					TextureEditorPropertiesWindow() = default;
					TextureEditorPropertiesWindow(EditorTab* editorTab);
					~TextureEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
