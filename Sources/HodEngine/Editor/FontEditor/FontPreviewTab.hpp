#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Math/Vector2.hpp>

#include <memory>

namespace hod::inline renderer
{
	class FontChain;
}

namespace hod::inline editor
{
	/// @brief A tab previewing text drawn as the engine draws it: the one of a face (FontFaceEditorTab), the one of a font (FontEditorTab).
	/// Its text, its metrics, its zoom and pan, in its menu bar. What it is drawn with is the tab's: see UpdateFontChain
	class HOD_EDITOR_API FontPreviewTab : public EditorTab
	{
	public:

		static constexpr float MinZoomFactor = 0.25f;
		static constexpr float MaxZoomFactor = 32.0f; // the pixels of a pixel art font big enough to tell them apart

	public:
				FontPreviewTab(std::shared_ptr<Asset> asset, const char* icon);
				~FontPreviewTab() override = default;

		// The faces the preview is drawn with, into the chain of a window: true when it changed (see FontChain::Update)
		virtual bool					UpdateFontChain(FontChain& fontChain) const = 0;

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

		void							InvalidatePreview(); // what it is drawn with changed

		void							DrawMenuBar() override;
		void							DrawMenuBarPopups() override;
		virtual void					DrawMenuBarOptions() {} // before the text of the preview: what the tab adds

	private:

		static constexpr float DefaultMargin = 16.0f; // between the viewport and the text, in its pixels

	private:

		char							_previewText[256] = "The quick brown fox jumps over the lazy dog";
		uint32_t						_previewVersion = 0;
		bool							_showMetrics = true;

		float							_zoomFactor = 1.0f;
		Vector2							_panOffset = Vector2(DefaultMargin, -DefaultMargin);
	};
}
