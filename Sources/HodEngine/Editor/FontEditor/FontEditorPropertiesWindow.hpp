#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline editor
{
	/// @brief The settings of the font (how its glyphs are drawn), then what the font file says about itself
	class HOD_EDITOR_API FontEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontEditorPropertiesWindow, EditorTabWindow)

	public:

					FontEditorPropertiesWindow() = default;
					FontEditorPropertiesWindow(EditorTab* editorTab);
					~FontEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawSettings();
		void		DrawInformation();
		bool		DetectPixelSize(); // into the settings, true when found

	private:

		String		_detectMessage; // what the last "Detect" of the pixel size found, shown under it
	};
}
