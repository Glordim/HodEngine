#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/Text/TextMesh.hpp>
#include <HodEngine/Text/TextLayout.hpp>

namespace hod::inline ui2
{
	/// @brief Draws a single line of text, aligned within the node's rect. Its natural size (see
	/// MeasureContent) is the text's bounds: the node fits its text on the axes its LayoutParams size
	/// from the content (LayoutParams::SizeMode::Content).
	class HOD_UI2_API TextNode : public Node
	{
		REFLECTED_CLASS(TextNode, Node)

	public:

		enum class Alignment : uint8_t
		{
			TopLeft,
			TopCenter,
			TopRight,
			MiddleLeft,
			MiddleCenter,
			MiddleRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
		};
		REFLECTED_ENUM(HOD_UI2_API, Alignment);

	public:

		const WeakResource<FontResource>&	GetFont() const;
		void								SetFont(const WeakResource<FontResource>& font);

		const String&						GetValue() const;
		void								SetValue(const String& value);

		// Of the em, in pixels of the canvas
		float								GetFontSize() const;
		void								SetFontSize(float fontSize);

		Alignment							GetAlignment() const;
		void								SetAlignment(Alignment alignment);

		const Color&						GetColor() const;
		void								SetColor(const Color& color);

		Vector2								MeasureContent(const Vector2& availableSize) const override;

	protected:

		void								DrawSelf(DrawContext& drawContext) override;

	private:

		// The value shaped with the font at the size, again when one of them changed. Null without a font
		const TextLayout*					UpdateLayout() const;
		Vector2								GetLayoutSize() const; // of the line: its width, from its ascender to its descender

	private:

		WeakResource<FontResource>			_font;
		String								_value;
		float								_fontSize = 48.0f;
		Alignment							_alignment = Alignment::MiddleCenter;
		Color								_color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		// Kept from one frame to the next, made again when what they are made from changes
		mutable TextLayout					_layout;
		mutable uint32_t					_layoutFontFaceId = 0;
		mutable bool						_layoutDirty = true;

		TextMesh							_mesh;
		Vector2								_meshOrigin;
		mutable bool						_meshDirty = true; // set by UpdateLayout too
	};
}
