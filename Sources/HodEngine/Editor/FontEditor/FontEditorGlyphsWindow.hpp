#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>
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
	/// @brief The characters of a Unicode block (Basic Latin, Cyrillic...), or every character of the font, in a grid: the ones the font lacks highlighted.
	/// Or the characters searched for: typed ("é", "中文"), or by their code ("U+00E9", "0x4E2D", "4E2D").
	/// Drawn by the engine, as the font draws them
	class HOD_EDITOR_API FontEditorGlyphsWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontEditorGlyphsWindow, EditorTabWindow)

	public:

					FontEditorGlyphsWindow() = default;
					FontEditorGlyphsWindow(EditorTab* editorTab);
					~FontEditorGlyphsWindow() override;

	public:

		void		DrawContent() override;

	private:

		struct Cell
		{
			char32_t	_codepoint = 0;
			uint32_t	_glyphId = 0; // 0: the font lacks it
		};

		static constexpr float CellSize = 64.0f;
		static constexpr float LabelHeight = 16.0f; // under the glyph: its codepoint
		static constexpr float GlyphSize = 32.0f;   // the em of the glyphs, PixelArt: the closest whole multiple of its grid

	private:

		void		DrawToolbar();
		void		UpdateCells(const FontFace& face);

	private:

		RenderTarget*	_renderTarget = nullptr;
		TextMesh		_mesh;

		uint32_t		_characterSetIndex = 1; // Basic Latin
		bool			_onlyMissing = false;
		char			_search[128] = {}; // the cells are its characters when it is not empty

		Vector<Cell>	_cells; // of the set, the missing ones only when _onlyMissing
		uint32_t		_characterCount = 0; // of the set
		uint32_t		_missingCount = 0;

		uint32_t		_builtFontFaceId = 0;
		uint32_t		_builtCharacterSetIndex = std::numeric_limits<uint32_t>::max();
		bool			_builtOnlyMissing = false;
		String			_builtSearch;
	};
}
