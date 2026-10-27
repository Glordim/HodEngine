#pragma once
#include "HodEngine/RendererEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline editor
{
	/// @brief The fonts the face is a style of, the settings of the face (how its glyphs are drawn), then what the font file says about itself
	class HOD_RENDERER_EDITOR_API FontFaceEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontFaceEditorPropertiesWindow, EditorTabWindow)

	public:

					FontFaceEditorPropertiesWindow() = default;
					FontFaceEditorPropertiesWindow(EditorTab* editorTab);
					~FontFaceEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawFonts();
		void		DrawSettings();
		void		DrawInformation();
		bool		DetectPixelSize(); // into the settings, true when found

	private:

		String		_detectMessage; // what the last "Detect" of the pixel size found, shown under it
		String		_createMessage; // what the last "Create Font" made
	};
}
