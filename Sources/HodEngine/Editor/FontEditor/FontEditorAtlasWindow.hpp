#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline editor
{
	/// @brief The pages of the glyph atlas of the font: the glyphs rasterized so far, in every size and mode it was drawn in.
	/// Zoomed and panned like the texture viewer
	class HOD_EDITOR_API FontEditorAtlasWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontEditorAtlasWindow, EditorTabWindow)

	public:

					FontEditorAtlasWindow() = default;
					FontEditorAtlasWindow(EditorTab* editorTab);
					~FontEditorAtlasWindow() override;

	public:

		void		DrawContent() override;

	private:

		static constexpr float MinZoomFactor = 0.125f;
		static constexpr float MaxZoomFactor = 32.0f;

	private:

		void		DrawToolbar(uint32_t pageCount);
		void		HandleInput(const Vector2& viewportPosition, const Vector2& viewportSize);

	private:

		RenderTarget*	_renderTarget = nullptr;

		uint32_t		_pageIndex = 0;
		bool			_outlineFontGlyphs = true;

		float			_zoomFactor = 1.0f;     // texels to pixels
		Vector2			_panOffset;             // of the center of the page, from the center of the view, in its pixels (y down)
		bool			_fitRequested = true;   // the page fitted in the view at the next frame (the first one, the Fit button)
	};
}
