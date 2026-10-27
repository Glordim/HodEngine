#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>
#include <HodEngine/Renderer/Text/TextMesh.hpp>

#include <cstdint>
#include <limits>

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline editor
{
	/// @brief The characters of a Unicode block (Basic Latin, Cyrillic...), or every character of the font, in a grid: the ones the font lacks highlighted,
	/// the ones a fallback draws apart from the ones none does.
	/// Or the characters searched for: typed ("é", "中文"), or by their code ("U+00E9", "0x4E2D", "4E2D").
	/// Drawn by the engine, as the font draws them
	class HOD_EDITOR_API FontPreviewGlyphsWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontPreviewGlyphsWindow, EditorTabWindow)

	public:

					FontPreviewGlyphsWindow() = default;
					FontPreviewGlyphsWindow(EditorTab* editorTab);
					~FontPreviewGlyphsWindow() override;

	public:

		void		DrawContent() override;

	private:

		struct Cell
		{
			char32_t	_codepoint = 0;
			uint32_t	_glyphId = 0;   // 0: neither the font nor its fallbacks have it
			uint32_t	_fontIndex = 0; // in the chain of the font: 0 the font, more a fallback
		};

		static constexpr float CellSize = 64.0f;
		static constexpr float LabelHeight = 16.0f; // under the glyph: its codepoint
		static constexpr float GlyphSize = 32.0f;   // the em of the glyphs, PixelArt: the closest whole multiple of its grid

	private:

		void		DrawToolbar();
		void		UpdateCells();

	private:

		RenderTarget*	_renderTarget = nullptr;
		FontChain		_fontChain; // the font, then its fallbacks
		TextMesh		_mesh;

		uint32_t		_characterSetIndex = 1; // Basic Latin
		bool			_onlyMissing = false;
		char			_search[128] = {}; // the cells are its characters when it is not empty

		Vector<Cell>	_cells; // of the set, the missing ones only when _onlyMissing
		uint32_t		_characterCount = 0; // of the set
		uint32_t		_fallbackCount = 0;  // the font lacks, a fallback has
		uint32_t		_missingCount = 0;   // none has

		uint32_t		_builtCharacterSetIndex = std::numeric_limits<uint32_t>::max();
		bool			_builtOnlyMissing = false;
		String			_builtSearch;
	};
}
