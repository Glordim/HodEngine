#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/FontEditor/FontPreviewTab.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceCooker.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <memory>

namespace hod::inline renderer
{
	class FontFaceResource;
}

namespace hod::inline editor
{
	/// @brief The settings of a face (how its glyphs are drawn), its information, and a preview of its text as the engine draws it.
	/// The settings are applied to the FontFaceResource as they are edited: whatever uses it follows. Reverted if the tab is closed unsaved.
	/// A text is drawn with a font, not a face: the fonts it is a style of are shown, a font made from it
	class HOD_EDITOR_API FontFaceEditorTab : public FontPreviewTab
	{
	public:
				FontFaceEditorTab(std::shared_ptr<Asset> asset);
				~FontFaceEditorTab() override;

		std::shared_ptr<FontFaceResource>		GetFontFace() const;

		FontFaceSettings&						GetFontFaceSettings();
		void									ApplySettings(); // to the resource, wherever it is used

		bool									UpdateFontChain(FontChain& fontChain) const override; // the face alone

		// The fonts it is a style of, as they were when looked for
		const Vector<std::shared_ptr<Asset>>&	GetFonts() const;
		void									FindFonts();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		bool	OnSave() override;

	private:

		FontFaceSettings						_fontFaceSettings;
		FontFaceSettings						_savedSettings; // what the resource is reverted to if the tab is closed with unsaved live edits
		std::shared_ptr<FontFaceResource>		_fontFace;
		Vector<std::shared_ptr<Asset>>			_fonts;
	};
}
