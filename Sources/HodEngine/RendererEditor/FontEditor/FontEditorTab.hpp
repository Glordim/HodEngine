#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/RendererEditor/FontEditor/FontCooker.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontPreviewTab.hpp"

#include <memory>

namespace hod::inline renderer
{
	class FontResource;
}

namespace hod::inline editor
{
	/// @brief The family of a font: its faces for each style, its fallbacks, and a preview of its text in a style as the engine draws it.
	/// The settings are applied to the FontResource as they are edited: whatever uses it follows. Reverted if the tab is closed unsaved.
	class HOD_RENDERER_EDITOR_API FontEditorTab : public FontPreviewTab
	{
	public:
				FontEditorTab(std::shared_ptr<Asset> asset);
				~FontEditorTab() override;

		std::shared_ptr<FontResource>	GetFont() const;

		FontSettings&					GetFontSettings();
		void							ApplySettings(); // to the resource, wherever it is used

		bool							UpdateFontChain(FontChain& fontChain) const override; // the font in the style of the preview

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBarOptions() override;
		bool	OnSave() override;

	private:

		FontSettings					_fontSettings;
		FontSettings					_savedSettings; // what the resource is reverted to if the tab is closed with unsaved live edits
		std::shared_ptr<FontResource>	_font;

		FontResource::Weight			_previewWeight = FontResource::Weight::Regular;
		bool							_previewItalic = false;
	};
}
