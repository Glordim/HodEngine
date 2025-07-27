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
	class HOD_EDITOR_API TextureEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(TextureEditorViewportWindow, EditorTabWindow)

	public:

					TextureEditorViewportWindow() = default;
					TextureEditorViewportWindow(EditorTab* editorTab);
					~TextureEditorViewportWindow() override = default;

	public:

		bool		Draw() override;
		void		DrawContent() override;
	};
}
