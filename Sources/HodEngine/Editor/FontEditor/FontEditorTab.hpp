#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/FontEditor/FontCooker.hpp"

#include <HodEngine/Math/Vector2.hpp>

#include <memory>

namespace hod::inline renderer
{
	class FontResource;
}

namespace hod::inline editor
{
	/// @brief The settings of a font (how its glyphs are drawn), its information, and a preview of its text as the engine draws it.
	/// The settings are applied to the FontResource as they are edited: whatever uses it follows. Reverted if the tab is closed unsaved.
	class HOD_EDITOR_API FontEditorTab : public EditorTab
	{
	public:

		static constexpr float MinZoomFactor = 0.25f;
		static constexpr float MaxZoomFactor = 32.0f; // the pixels of a pixel art font big enough to tell them apart

	public:
				FontEditorTab(std::shared_ptr<Asset> asset);
				~FontEditorTab() override;

		std::shared_ptr<FontResource>	GetFont() const;

		FontSettings&					GetFontSettings();
		void							ApplySettings(); // to the resource, wherever it is used

		// Changes each time what the preview draws changes: its text, the settings
		uint32_t						GetPreviewVersion() const;
		const char*						GetPreviewText() const;

		bool							IsShowingMetrics() const;

		float							GetZoomFactor() const;
		void							SetZoomFactor(float zoomFactor);
		// Of the top left of the text, from the top left of the viewport, in its pixels (x right, y up)
		const Vector2&					GetPanOffset() const;
		void							SetPanOffset(const Vector2& panOffset);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		void	DrawMenuBarPopups() override;
		bool	OnSave() override;

	private:

		static constexpr float DefaultMargin = 16.0f; // between the viewport and the text, in its pixels

	private:

		FontSettings					_fontSettings;
		FontSettings					_savedSettings; // what the resource is reverted to if the tab is closed with unsaved live edits
		std::shared_ptr<FontResource>	_font;

		char							_previewText[256] = "The quick brown fox jumps over the lazy dog";
		uint32_t						_previewVersion = 0;
		bool							_showMetrics = true;

		float							_zoomFactor = 1.0f;
		Vector2							_panOffset = Vector2(DefaultMargin, -DefaultMargin);
	};
}
