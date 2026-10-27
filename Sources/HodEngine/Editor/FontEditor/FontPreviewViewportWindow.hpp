#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>
#include <HodEngine/Renderer/Text/TextMesh.hpp>
#include <HodEngine/Text/TextLayout.hpp>

#include <cstdint>
#include <limits>

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline renderer
{
	class FontFaceResource;
}

namespace hod::inline editor
{
	/// @brief The text of the preview at a waterfall of sizes, drawn by the engine (TextShaper, GlyphCache, TextMesh) as a TextNode draws it,
	/// with the settings being edited. Zoomed and panned like the texture viewer, to look at the pixels.
	class HOD_EDITOR_API FontPreviewViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(FontPreviewViewportWindow, EditorTabWindow)

	public:

					FontPreviewViewportWindow() = default;
					FontPreviewViewportWindow(EditorTab* editorTab);
					~FontPreviewViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		// The text at one size, laid out from the top left of the preview (x right, y up)
		struct PreviewLine
		{
			float		_fontSize = 0.0f;
			String		_label;
			TextLayout	_layout;
			TextMesh	_mesh;
			float		_baseline = 0.0f;
			float		_xHeight = 0.0f;
		};

		static constexpr float LabelColumnWidth = 96.0f; // the text starts after the labels of the sizes
		static constexpr float LineSpacing = 12.0f;

	private:

		void		BuildLines(FontFaceResource& font, const char* text);
		void		HandleInput(const Vector2& viewportPosition, const Vector2& viewportSize);
		void		DrawMetrics(const Vector2& viewportPosition, const Vector2& viewportSize, const FontFaceResource& font);

	private:

		RenderTarget*		_renderTarget = nullptr;

		Vector<PreviewLine>	_lines;
		FontChain			_fontChain; // the lines are drawn with it, see FontPreviewTab::UpdateFontChain
		uint32_t			_builtPreviewVersion = std::numeric_limits<uint32_t>::max();
	};
}
