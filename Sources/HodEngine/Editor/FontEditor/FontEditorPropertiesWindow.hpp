#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>

namespace hod::inline editor
{
	/// @brief The family of the font: its faces for each style, its fallbacks, and the faces the preview is drawn with
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

		void		DrawStyles();
		void		DrawFallbacks();
		void		DrawChain();

	private:

		String		_detectMessage; // what the last "Detect" of the styles found, shown under it
		FontChain	_fontChain;     // the one of the preview, shown at the bottom
	};
}
