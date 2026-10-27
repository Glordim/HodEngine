#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>
#include <HodEngine/Renderer/Text/TextMesh.hpp>
#include <HodEngine/Text/TextLayout.hpp>

namespace hod::inline ui2
{
	/// @brief Draws a single line of text, aligned within the node's rect. Its natural size (see
	/// MeasureContent) is the text's bounds: the node fits its text on the axes its LayoutParams size
	/// from the content (LayoutParams::SizeMode::Content). On the other axes the text may not fit: see Overflow.
	class HOD_UI2_API TextNode : public Node
	{
		REFLECTED_CLASS(TextNode, Node)

	public:

		enum class HorizontalAlignment : uint8_t
		{
			Left,
			Center,
			Right,
		};
		REFLECTED_ENUM(HOD_UI2_API, HorizontalAlignment);

		/// @brief What is aligned is the line (from the ascender to the descender of the font): the baseline does not depend on the text.
		/// See AlignByGeometry for its ink instead.
		enum class VerticalAlignment : uint8_t
		{
			Top,
			Middle,
			Capline, // the middle of the capitals (from the baseline to the cap height) in the middle of the rect: what looks centered
			Bottom,
		};
		REFLECTED_ENUM(HOD_UI2_API, VerticalAlignment);

		/// @brief What is done with a text that does not fit in the rect of the node, on the axes not sized from the content
		enum class Overflow : uint8_t
		{
			Overflow, // drawn whole, out of the rect
			Clip,     // drawn whole, what is out of the rect is not
			Ellipsis, // cut after the last character that fits in the width, with "…" after it
			Shrink,   // the font made smaller, down to MinFontSize, until it fits. Then with an ellipsis in the width, and clipped in the height
		};
		REFLECTED_ENUM(HOD_UI2_API, Overflow);

	public:

		const WeakResource<FontResource>&	GetFont() const;
		void								SetFont(const WeakResource<FontResource>& font);

		const String&						GetValue() const;
		void								SetValue(const String& value);

		// Of the em, in pixels of the canvas. A PixelArt font is drawn at the closest whole multiple of its PixelSize
		float								GetFontSize() const;
		void								SetFontSize(float fontSize);

		// The style drawn: the face of the font closest to it (see FontResource::FindStyle)
		FontResource::Weight				GetFontWeight() const;
		void								SetFontWeight(FontResource::Weight fontWeight);
		bool								IsItalic() const;
		void								SetItalic(bool italic);

		HorizontalAlignment					GetHorizontalAlignment() const;
		void								SetHorizontalAlignment(HorizontalAlignment horizontalAlignment);

		VerticalAlignment					GetVerticalAlignment() const;
		void								SetVerticalAlignment(VerticalAlignment verticalAlignment);

		// Aligns the ink of the text (the bounds of what is drawn) instead of its line (its advances, the ascender and descender of the font):
		// exactly centered, but the baseline moves with the text. Capline is Middle then
		bool								GetAlignByGeometry() const;
		void								SetAlignByGeometry(bool alignByGeometry);

		Overflow							GetOverflow() const;
		void								SetOverflow(Overflow overflow);

		// Overflow::Shrink only. A PixelArt font is shrunk by whole multiples of its PixelSize, down to the closest one to this
		float								GetMinFontSize() const;
		void								SetMinFontSize(float minFontSize);

		const Color&						GetColor() const;
		void								SetColor(const Color& color);

		Vector2								MeasureContent(const Vector2& availableSize) const override;

	protected:

		void								DrawSelf(DrawContext& drawContext) override;

	private:

		// The value shaped with the font at the size, again when one of them changed. Null without a font
		const TextLayout*					UpdateLayout() const;
		// The layout fitted in the rect of the node, as Overflow says, again when the layout or the rect changed. Null without a font
		const TextLayout*					UpdateFittedLayout(const Vector2& size);

		static Vector2						GetLayoutSize(const TextLayout& layout); // of the line: its width, from its ascender to its descender
		bool								IsPixelArt() const;   // the font of the layout, its style (not its fallbacks)
		uint32_t							GetPixelSize() const; // its grid, PixelArt
		// A glyph of the layout is from a Bitmap or Pixel font: its glyphs on whole units. Sdf ones go anywhere, at any size.
		// The faces of the chain the text does not use (fallbacks...) don't count
		bool								IsDrawnOnPixels(const TextLayout& layout) const;

	private:

		WeakResource<FontResource>			_font;
		String								_value;
		float								_fontSize = 48.0f;
		FontResource::Weight				_fontWeight = FontResource::Weight::Regular;
		bool								_italic = false;
		HorizontalAlignment					_horizontalAlignment = HorizontalAlignment::Center;
		VerticalAlignment					_verticalAlignment = VerticalAlignment::Middle;
		bool								_alignByGeometry = false;
		Overflow							_overflow = Overflow::Overflow;
		float								_minFontSize = 8.0f;
		Color								_color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		// Kept from one frame to the next, made again when what they are made from changes
		mutable FontChain					_fontChain; // the font (its style), its fallbacks: the layout is shaped with it
		mutable TextLayout					_layout;
		mutable float						_layoutFontSize = 0.0f; // the one it is drawn at, see FontFaceResource::GetDrawnFontSize
		mutable bool						_layoutDirty = true;

		TextLayout							_fittedLayout;
		float								_fittedFontSize = 0.0f;
		Vector2								_fittedSize; // of the rect it is fitted in
		mutable bool						_fittedDirty = true; // set by UpdateLayout too

		TextMesh							_mesh; // from the start of the baseline: placed by the alignment when drawn
		bool								_meshDirty = true;
	};
}
